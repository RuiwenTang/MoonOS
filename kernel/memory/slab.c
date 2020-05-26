#include <moonos/kprintf.h>
#include <moonos/memory.h>
#include <moonos/memory/buddy.h>
#include <moonos/memory/slab.h>

struct list {
    struct list* next;
};
typedef struct list list_t;

struct slab {
    list_head_t ll;
    page_t* page;
    list_t* free;
    size_t size;
};
typedef struct slab slab_t;

static slab_t* slab_get_meta(slab_cache_t* cache, void* virt) {
    const size_t bytes = (size_t)PAGE_SIZE << cache->slab_order;

    return (slab_t*)((char*)virt + bytes - sizeof(slab_t));
}

static slab_t* slab_create(slab_cache_t* cache) {
    page_t* page = __buddy_alloc(cache->slab_order);

    if (!page) {
        return 0;
    }

    const uintptr_t phys = page_addr(page);
    char* ptr = (char*)va(phys);
    slab_t* slab = slab_get_meta(cache, ptr);
    list_t* head = 0;

    slab->page = page;
    slab->size = cache->slab_size;

    for (size_t i = 0; i != slab->size; i++, ptr += cache->obj_size) {
        list_t* node = (list_t*)ptr;

        node->next = head;
        head = node;
    }
    slab->free = head;
    return slab;
}

static void slab_destroy(slab_cache_t* cache, slab_t* slab) {
    __buddy_free(slab->page, cache->slab_order);
}

static void* slab_alloc(slab_t* slab) {
    list_t* head = slab->free;
    void* ptr = head;

    slab->free = head->next;
    slab->size--;
    return ptr;
}

static void slab_free(slab_t* slab, void* ptr) {
    list_t* head = (list_t*)ptr;

    head->next = slab->free;
    slab->free = head;
    slab->size++;
}

static size_t slab_align_down(size_t x, size_t align) { return x - x % align; }
static size_t slab_align_up(size_t x, size_t align) {
    return slab_align_down(x + align - 1, align);
}

void __slab_cache_setup(slab_cache_t* cache, size_t size, size_t align) {
    if (size < sizeof(list_t)) {
        size = sizeof(list_t);
    }

    if (size > (size_t)PAGE_SIZE << MAX_ORDER) {
        kprintf("Object size is to large to allocate (%llu)\n", (uint64_t)size);
        while (1)
            ;
    }

    const size_t obj_size = slab_align_up(size, align);

    /**
     * We select slab_order so that amortized allocation time will be O(1).
     * Since we use buddy allocator to allocate slabs which works in
     * O(MAX_ORDER) time (plus time required to check a zone), one slab
     * should consist of at least MAX_ORDER objects;
     **/
    size_t bytes;
    int slab_order;
    for (slab_order = 0; slab_order <= MAX_ORDER; slab_order++) {
        bytes = ((size_t)PAGE_SIZE << slab_order) - sizeof(slab_t);
        if (bytes / obj_size >= MAX_ORDER) {
            break;
        }
    }
    const size_t slab_size = bytes / obj_size;

    list_init(&cache->full);
    list_init(&cache->partial);
    list_init(&cache->empty);

    cache->slab_order = slab_order;
    cache->slab_size = slab_size;
    cache->obj_size = obj_size;
}

void slab_cache_setup(slab_cache_t* cache, size_t size) {
    const size_t DEFAULT_ALIGN = 8;

    __slab_cache_setup(cache, size, DEFAULT_ALIGN);
}

void slab_cache_release(struct slab_cache* cache) {
    if (!list_empty(&cache->full) || !list_empty(&cache->partial)) {
        kprintf("Slab cache still contains busy objects\n");
        while (1)
            ;
    }

    slab_cache_shrink(cache);
}

void slab_cache_shrink(slab_cache_t* cache) {
    list_head_t list;
    list_head_t* head = &list;

    list_init(&list);
    list_splice(&cache->empty, &list);

    for (list_head_t* ptr = head->next; ptr != head;) {
        slab_t* slab = (slab_t*)ptr;

        ptr = ptr->next;
        slab_destroy(cache, slab);
    }
}

void* slab_cache_alloc(slab_cache_t* cache) {
    slab_t* slab = 0;
    if (!list_empty(&cache->partial)) {
        slab = (slab_t*)cache->partial.next;
        list_del(&slab->ll);
    } else if (!list_empty(&cache->empty)) {
        slab = (slab_t*)cache->empty.next;
        list_del(&slab->ll);
    } else {
        slab = slab_create(cache);
        if (!slab) {
            return 0;
        }
    }

    void* ptr = slab_alloc(slab);

    if (slab->size) {
        list_add(&slab->ll, &cache->partial);
    } else {
        list_add(&slab->ll, &cache->full);
    }
    return ptr;
}

void slab_cache_free(slab_cache_t* cache, void* ptr) {
    const size_t bytes = (size_t)PAGE_SIZE << cache->slab_order;
    const uintptr_t addr = ~(bytes - 1) & (uintptr_t)ptr;

    slab_t* slab = slab_get_meta(cache, (void*)addr);

    list_del(&slab->ll);
    slab_free(slab, ptr);

    if (slab->size == cache->slab_size) {
        list_add(&slab->ll, &cache->empty);
    } else {
        list_add(&slab->ll, &cache->partial);
    }
}
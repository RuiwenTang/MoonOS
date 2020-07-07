#include <moonos/memory/buddy.h>
#include <moonos/memory/slab.h>
#include <moonos/ramfs/ramfs.h>
#include <moonos/thread/mutex.h>
#include <string.h>

struct ramfs_page {
    list_head_t ll;
    long offs;
    page_t* page;
};

static slab_cache_t ramfs_slab;
static slab_cache_t page_slab;
static list_head_t ramfs_cache;
static struct mutex ramfs_mtx;

static int __ramfs_open(const char* name, int create, struct file** res) {
    list_head_t* head = &ramfs_cache;

    for (list_head_t* ptr = head->next; ptr != head; ptr = ptr->next) {
        struct file* file = (struct file*)ptr;
        if (strcmp(name, file->name)) {
            continue;
        }

        *res = file;
        return 0;
    }

    if (!create) {
        return -1;
    }

    struct file* new_file = slab_cache_alloc(&ramfs_slab);
    new_file->size = 0;
    strcpy(new_file->name, name);
    list_add(&new_file->ll, head);
    list_init(&new_file->data);
    *res = new_file;
    return 0;
}

static long __ramfs_readat(struct file* file, char* data, long size,
                           long offs) {
    if (offs >= file->size) {
        return 0;
    }

    list_head_t* head = &(file->data);

    const long from = offs;
    const long to = offs + size;

    for (list_head_t* ptr = head->next; ptr != head; ptr = ptr->next) {
        struct ramfs_page* rp = (struct ramfs_page*)ptr;

        if (rp->offs + PAGE_SIZE <= from) {
            continue;
        }

        if (rp->offs >= to) {
            break;
        }

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) < (b) ? (b) : (a))

        const long data_offs = MAX(rp->offs - from, 0);
        const long page_offs = MAX(offs - rp->offs, 0);
        const long to_copy = MIN(PAGE_SIZE - page_offs, to - data_offs);
        const char* page = va(page_addr(rp->page));

#undef MIN
#undef MAX

        memcpy(data + data_offs, page + page_offs, to_copy);
    }

    return to >= file->size ? file->size - offs : size;
}

static long __ramfs_writeat(struct file* file, const char* data, long size,
                            long offs) {
    list_head_t* head = &file->data;
    const long from = offs;
    const long to = offs + size;

    if (to > file->size) {
        file->size = to;
    }

    for (list_head_t* ptr = head->next; offs != to; ptr = ptr->next) {
        struct ramfs_page* rp = (struct ramfs_page*)ptr;
        if (ptr != head && rp->offs + PAGE_SIZE <= offs) {
            continue;
        }

        if (ptr == head || rp->offs > offs) {
            struct ramfs_page* new_page = slab_cache_alloc(&page_slab);
            if (!new_page) {
                return offs == from ? -1 : offs - from;
            }

            new_page->page = __buddy_alloc(0);
            if (!new_page->page) {
                slab_cache_free(&page_slab, new_page);
                return offs == from ? -1 : offs - from;
            }

            // all offsets must be PAGE_SIZE aligned
            new_page->offs = offs & ~(PAGE_SIZE - 1l);
            list_add_before(&new_page->ll, ptr);
            ptr = &new_page->ll;
            rp = new_page;
        }
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) < (b) ? (b) : (a))
        const long data_offs = MAX(rp->offs - from, 0);
        const long page_offs = MAX(offs - rp->offs, 0);
        const long to_copy = MIN(PAGE_SIZE - page_offs, to - data_offs);
        char* page = va(page_addr(rp->page));
#undef MAX
#undef MIN

        memcpy(page + page_offs, data + data_offs, to_copy);
        offs += to_copy;
    }
    return offs - from;
}

int ramfs_create(const char* name, struct file** res) {
    int err;

    mutex_lock(&ramfs_mtx);
    err = __ramfs_open(name, 1, res);
    mutex_unlock(&ramfs_mtx);
    return err;
}

int ramfs_open(const char* name, struct file** res) {
    int err;

    mutex_lock(&ramfs_mtx);
    err = __ramfs_open(name, 0, res);
    mutex_unlock(&ramfs_mtx);
    return err;
}

/**
 * @brief not implement
 *
 * @param file file tobe closed
 */
void ramfs_close(struct file* file) { (void)file; }

long ramfs_readat(struct file* file, void* data, long size, long offs) {
    long ret;

    memset(data, 0, size);
    mutex_lock(&ramfs_mtx);
    ret = __ramfs_readat(file, data, size, offs);
    mutex_unlock(&ramfs_mtx);

    return ret;
}

long ramfs_writeat(struct file* file, const void* data, long size, long offs) {
    long ret;

    mutex_lock(&ramfs_mtx);
    ret = __ramfs_writeat(file, data, size, offs);
    mutex_unlock(&ramfs_mtx);
    return ret;
}

void ramfs_setup(void) {
    slab_cache_setup(&ramfs_slab, sizeof(struct file));
    slab_cache_setup(&page_slab, sizeof(struct ramfs_page));
    mutex_setup(&ramfs_mtx);
    list_init(&ramfs_cache);
}

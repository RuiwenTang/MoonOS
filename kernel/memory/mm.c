#include <moonos/memory/mm.h>
#include <moonos/memory/paging.h>
#include <moonos/memory/slab.h>
#include <string.h>

static const uint64_t USER_MASK = 0x000fffffffffffffull;
static slab_cache_t mm_slab;
static slab_cache_t vma_slab;

mm_t* mm_create(void) {
    mm_t* mm = (mm_t*)slab_cache_alloc(&mm_slab);
    if (!mm) {
        return 0;
    }

    list_init(&mm->vmas);
    mm->pt = __buddy_alloc(0);
    if (!mm->pt) {
        slab_cache_free(&mm_slab, mm);
        return 0;
    }

    mm->cr3 = page_addr(mm->pt);

    /**
     * kernel part of the mapping is the same for every process
     * and never changes so we need to copy it from the initial
     **/
    const size_t offs = pt_index(HIGHER_BASE, 4) * sizeof(pte_t);
    uint8_t* ptr = va(mm->cr3);
    
    memset(ptr, 0, PAGE_SIZE);
    memcpy(ptr + offs, va(initial_cr3 + offs), PAGE_SIZE - offs);

    return mm;
}

void mm_release(mm_t* mm) {
    
}
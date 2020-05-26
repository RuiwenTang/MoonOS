#ifndef __SLAB_H__
#define __SLAB_H__

#include <moonos/util/list.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This is a simplified version of classical SLAB allocator algorithm.
 * Simplified because we dropped some of the optimisation described in
 * the original paper, specifically:
 *  - cache coloring
 *  - constructors/destructors
 **/

struct slab_cache {
    list_head_t full;
    list_head_t partial;
    list_head_t empty;

    int slab_order;
    size_t slab_size;
    size_t obj_size;
};

typedef struct slab_cache slab_cache_t;

void __slab_cache_setup(slab_cache_t* cache, size_t size, size_t align);
void slab_cache_setup(slab_cache_t* cache, size_t);
void slab_cache_release(slab_cache_t* cache);

void slab_cache_shrink(slab_cache_t* cache);
void* slab_cache_alloc(slab_cache_t* cache);
void slab_cache_free(slab_cache_t* cache, void* ptr);

#ifdef __cplusplus
}
#endif

#endif  // __SLAB_H__
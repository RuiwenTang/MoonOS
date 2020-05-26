#ifndef __BUDDY_H__
#define __BUDDY_H__

#include <moonos/memory.h>
#include <moonos/util/list.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Maximum possible allocation size 2^20 pages */
#define MAX_ORDER 20

struct page {
    list_head_t ll;
    unsigned long flags;
    int order;
};

typedef struct page page_t;

void buddy_setup(void);

/**
 * Buddy alloc/free routines are given in two versions:
 *  - one returns descriptor (struct page)
 *  - other resutrns physical address
 **/
page_t* __buddy_alloc(int order);
uintptr_t boddy_alloc(int order);
void __buddy_free(page_t* page, int order);
void buddy_free(uintptr_t phys, int order);

/* Convertion routines: descriptor to physical address and vice versa. */
uintptr_t page_addr(const page_t* page);
page_t* addr_page(uintptr_t phys);

#ifdef __cplusplus
}
#endif

#endif  // __BUDDY_H__
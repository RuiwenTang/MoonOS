#ifndef __MM_H__
#define __MM_H__

#include <moonos/util/list.h>
#include <moonos/memory/buddy.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum vma_access {
    VMA_ACCESS_READ,
    VMA_ACCESS_WRITE,
    VMA_ACCESS_EXECUTE,
};

enum vma_perm {
    VMA_PREM_READ = (1u << VMA_ACCESS_READ),
    VMA_PREM_WRITE = (1u << VMA_ACCESS_WRITE),
    VMA_PREM_EXECUTE = (1u << VMA_ACCESS_EXECUTE),
};

struct vma {
    list_head_t ll;
    uintptr_t begin;
    uintptr_t end;
    uint32_t perm;
};

struct mm {
    list_head_t vmas;

    struct page* pt;
    uintptr_t cr3;
};
typedef struct mm mm_t;

struct mm* mm_create(void);
void mm_release(mm_t* mm);
int mm_copy(mm_t* dst, mm_t* src);

int mmap(mm_t* mm, uintptr_t from, uintptr_t to, uint32_t perm);
int munmap(mm_t* mm, uintptr_t from, uintptr_t to);

int mcopy(mm_t* dst, uintptr_t to, mm_t * src, uintptr_t from, size_t size);
int mset(mm_t* dst, uintptr_t to, uint32_t c, size_t size);
void mm_setup(void);

#ifdef __cplusplus
}
#endif

#endif  // __MM_H__
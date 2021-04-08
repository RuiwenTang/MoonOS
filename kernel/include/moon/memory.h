/*******************************************************************************
 * Project: moon-os                                                            *
 * File Created: Monday, 2nd November 2020 9:38:33 pm                          *
 * Author: tangruiwen (tangruiwen1989@gmail.com)                               *
 * Copyright - 2020                                                            *
 ******************************************************************************/
#ifndef __MOONOS_MEMORY_H__
#define __MOONOS_MEMORY_H__

/* Higher 2 GB of 64 bit logical address space start from this address */
#define VIRTUAL_BASE 0xffffffff80000000

/* First address after "canonical hole", beginning of the middle mapping. */
#define HIGHER_BASE 0xffff800000000000

#define IO_VIRTUAL_BASE (VIRTUAL_BASE - 0x100000000)  // VIRTUAL_BASE - 4GB

/* Kernel 64 bit code and data segment selectors. */
#define KERNEL_CS 0x08
#define KERNEL_DS 0x10

#define PAGE_BITS 12
#define PAGE_SIZE (1 << PAGE_BITS)
#define PAGE_MASK (PAGE_SIZE - 1)

#ifndef __ASSEMBLER__

#include <stddef.h>
#include <stdint.h>

#define VA(x) ((void*)((uintptr_t)x + VIRTUAL_BASE))
#define PA(x) ((uintptr_t)x - VIRTUAL_BASE)

static inline void* va(uintptr_t phys) { return VA(phys); }

static inline uintptr_t pa(const void* virt) { return PA(virt); }

#endif /*__ASM_FILE__*/

#endif /*__MOONOS_MEMORY_H__*/

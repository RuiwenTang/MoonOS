/*******************************************************************************
 * Project: moon-os                                                            *
 * File Created: Thursday, 5th November 2020 1:09:22 pm                        *
 * Author: tangruiwen (tangruiwen1989@gmail.com)                               *
 * Copyright - 2020                                                            *
 ******************************************************************************/
#include <string.h>

#include <moon/idt.hpp>
#include <moon/paging.hpp>
#ifdef DEBUG
#include "kprintf.hpp"
#endif

extern uint64_t bootstrap_pml4[];
extern uint64_t bootstrap_pml3[];

#define KERNEL_HEAP_PDPT_INDEX 511
#define KERNEL_HEAP_PML4_INDEX 511

#define PML4_GET_INDEX(addr) (((addr) >> 39) & 0x1FF)
#define PDPT_GET_INDEX(addr) (((addr) >> 30) & 0x1FF)
#define PAGE_DIR_GET_INDEX(addr) (((addr) >> 21) & 0x1FF)
#define PAGE_TABLE_GET_INDEX(addr) (((addr) >> 12) & 0x1FF)
#define PDPE_1G_MASK 0x1FFFFFFF

static void PageFaultHandler(void*, void* regs) {}

static void set_page_frame(uint64_t* page, uint64_t addr) {
  *page = (*page & ~Paging::PAGE_FRAME) | (addr & Paging::PAGE_FRAME);
}

static void set_page_flags(uint64_t* page, uint64_t flags) { *page |= flags; }

static uint32_t get_page_frame(uint64_t p) {
  return (p & Paging::PAGE_FRAME) >> 12;
}

static void invlpg(uintptr_t addr) { asm("invlpg (%0)" ::"r"(addr)); }

Paging* Paging::Instance() {
  static Paging paging{};
  return &paging;
}

Paging::Paging() {
  memset(fKernelPML4, 0, sizeof(pml4_t));
  memset(fKernelPDPT, 0, sizeof(pdpt_t));
#ifdef DEBUG
  kprintf("self fKernelPML4 addr = %x\n", pa(fKernelPML4));
  kprintf("self kKernelPDPT addr = %x\n", pa(fKernelPDPT));
#endif
}

void Paging::InitVirtualMemory() {
  IDT::RegisterInterruptHandler(14, PageFaultHandler);
  set_page_frame(fKernelPML4 + PML4_GET_INDEX(VIRTUAL_BASE), pa(fKernelPDPT));
  fKernelPML4[PML4_GET_INDEX(VIRTUAL_BASE)] |= 0x3;

  fKernelPDPT[PDPT_GET_INDEX(VIRTUAL_BASE)] = 0 | 0x3 | (1 << 7);

  asm("mov %%rax, %%cr3" ::"a"(pa(fKernelPML4)));
#ifdef DEBUG

  for (size_t i = 0; i < 512; i++) {
    if (fKernelPML4[i] != 0) {
      kprintf("index = %d \n", i);
      kprintf("pml = %x | %x \n", ((fKernelPML4[i] >> 32) & 0xffffffff),
              (fKernelPML4[i] & 0xffffffff));
    }
  }
#endif
}
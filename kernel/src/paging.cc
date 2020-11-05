#include <string.h>

#include <moon/idt.hpp>
#include <moon/paging.hpp>

extern uintptr_t bootstrap_pml4;

#define KERNEL_HEAP_PDPT_INDEX 511
#define KERNEL_HEAP_PML4_INDEX 511

static void PageFaultHandler(void*, void* regs) {}

Paging* Paging::Instance() {
  static Paging paging{};
  return &paging;
}

Paging::Paging() : fKernelPML4((pml4_t)bootstrap_pml4) {}

void Paging::InitVirtualMemory() {
  IDT::RegisterInterruptHandler(14, PageFaultHandler);
  memset(fKernelPDPT, 0, sizeof(pdpt_t));
  memset(fKernelHeapDir, 0, sizeof(page_dir_t));
}
#pragma once

#include <moon/memory.h>

class Paging final {
 public:
  enum : uint64_t {
    PAGE_PRESENT = 1,
    PAGE_WRITABLE = 1 << 1,
    PAGE_USER = 1 << 2,
    PAGE_WRITETHROUGH = 1 << 3,
    PAGE_CACHE_DISABLED = 1 << 4,
    PAGE_FRAME = 0xFFFFFFFFFF000,
    PAGE_SIZE_4k = 4096,
    PAGE_SIZE_2M = 0x200000,
    PAGE_SIZE_1G = 0x40000000,
    PAGE_PDPT_SIZE = 0x8000000000,
    PAGE_PAGES_PER_TABLE = 512,
    PAGE_TABLES_PER_DIR = 512,
    PAGE_DIRS_PER_PDPT = 512,
    PAGE_PDPTS_PER_PML4 = 512,
    PAGE_MAX_PDPT_INDEX = 511,
  };

  using page_t = uint64_t;
  using pd_entry_t = uint64_t;
  using pdpt_entry_t = uint64_t;
  using pml4_entry_t = uint64_t;
  using page_dir_t = pd_entry_t[PAGE_TABLES_PER_DIR];
  using pdpt_t = pdpt_entry_t[PAGE_DIRS_PER_PDPT];
  using pml4_t = pml4_entry_t*;

  /**
   * @brief Process Address Info
   * Each process will have a maximum of 96GB of virtual memory.
   */
  struct AddressSpace {
    /**
     * @brief Pointer to the process pdpt address
     *  512GB is more than sample
     */
    pdpt_entry_t* pdpt;
    /**
     * @brief
     * [64]
     */
    pd_entry_t** pageDirs;
    uint64_t* pageDirsPhys;
    /// [64][TABLES_PER_DIR]
    page_t*** pageTables;
  };

 public:
  static Paging* Instance();
  void InitVirtualMemory();
  uintptr_t AllocateVirtualMemory(uint64_t size);
  uintptr_t KernelAllocateVirtualMemory(uint64_t size);
  void FreeVirtualMemory(uintptr_t pointer, uint64_t size);
  void KernelFree2MPages(uintptr_t addr, uint64_t amount);
  void KernelFree4KPages(uintptr_t addr, uint64_t amount);

 private:
  Paging();
  ~Paging() = default;

 private:
  pml4_t fKernelPML4;
  pdpt_t fKernelPDPT;
  page_dir_t fKernelDir;
  page_dir_t fKernelHeapDir;
  page_t kernelHeapDirTables[PAGE_TABLES_PER_DIR][PAGE_PAGES_PER_TABLE];
  page_dir_t fIODirs[4];
};
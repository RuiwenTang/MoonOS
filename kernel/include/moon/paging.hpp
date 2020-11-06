#pragma once

#include <moon/memory.h>

class Paging final {
 public:
  enum : uint64_t {
    PDE_PRESENT = 1,
    PDE_WRITABLE = 1 << 1,
    PDE_USER = 1 << 2,
    PDE_CACHE_DISABLED = 1 << 4,
    PDE_2M = 1 << 7,
    PDPE_2M = 1 << 7,
    PDE_FRAME = 0xFFFFFFFFFF000,
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
  using pml4_t = pml4_entry_t[PAGE_PDPTS_PER_PML4];

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

  struct PageTable {
    uint64_t phys;
    page_t* virt;
  };

 public:
  static Paging* Instance();
  void InitVirtualMemory();
  void* AllocateVirtualMemory(uint64_t size);
  void* KernelAllocateVirtualMemory(uint64_t size);
  void FreeVirtualMemory(void* pointer, uint64_t size);
  void KernelFree2MPages(void* addr, uint64_t amount);
  void KernelFree4KPages(void* addr, uint64_t amount);

  void* Allocate4KPages(uint64_t amount);
  void* Allocate4KPages(uint64_t, AddressSpace* addressSpace);
  void* Allocate2MPages(uint64_t amount);
  void* Allocate1GPages(uint64_t amount);

  void MapVirtualMemory4K(uint64_t phys, uint64_t virt, uint64_t amount);
  void MapVirtualMemory2M(uint64_t phys, uint64_t virt, uint64_t amount);
  void MapVirtualMemory1G(uint64_t phys, uint64_t virt, uint64_t amount);

  void KernelMapVirtualMemory4K(uint64_t phys, uint64_t virt, uint64_t amount);
  void KernelMapVirtualMemory2M(uint64_t phys, uint64_t virt, uint64_t amount);
  void KernelMapVirtualMemory1G(uint64_t phys, uint64_t virt, uint64_t amount);

 private:
  Paging();
  ~Paging() = default;

 private:
  pml4_t fKernelPML4 __attribute__((aligned(4096)));
  pdpt_t fKernelPDPT __attribute__((aligned(4096)));
  page_dir_t fKernelDir __attribute__((aligned(4096)));
  page_dir_t fKernelHeapDir __attribute__((aligned(4096)));
  page_t kernelHeapDirTables[PAGE_TABLES_PER_DIR][PAGE_PAGES_PER_TABLE]
      __attribute__((aligned(4096)));
  page_dir_t fIODirs[4] __attribute__((aligned(4096)));
};
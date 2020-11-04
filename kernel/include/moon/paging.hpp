#pragma once

#include <moon/memory.h>


namespace Paging {

enum : uint64_t{
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


}
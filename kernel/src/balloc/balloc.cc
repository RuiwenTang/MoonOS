/*******************************************************************************
 * Project: moon-os                                                            *
 * File Created: Sunday, 8th November 2020 1:29:49 pm                          *
 * Author: tangruiwen (tangruiwen1989@gmail.com)                               *
 * Copyright - 2020                                                            *
 ******************************************************************************/
#include "balloc.hpp"

#include <moon/memory.h>
#include <stdint.h>
#include <string.h>
#ifdef DEBUG
#include "../kprintf.hpp"
#endif

#define DEBUG_BALLOC

Balloc* Balloc::Instance() {
  static Balloc gBalloc{};
  return &gBalloc;
}

void Balloc::Init(multiboot_info_t* mb_info) {
  fMultibootInfo =
      static_cast<multiboot_info_t*>(va(reinterpret_cast<uintptr_t>(mb_info)));

  InitInternal();
}

void Balloc::InitInternal() {
  if ((fMultibootInfo->flags & MULTIBOOT_INFO_MEMORY) == 0) {
    return;
  }

  const uint64_t begin = fMultibootInfo->mmap_addr;
  const uint64_t end = begin + fMultibootInfo->mmap_length;
  uint64_t ptr = begin;
  while (ptr + sizeof(struct multiboot_mmap_entry) <= end) {
    multiboot_memory_map_t* entry =
        reinterpret_cast<multiboot_memory_map_t*>(va(ptr));
    uint64_t from = entry->addr;
    uint64_t to = from + entry->len;

    AddToRange(fAllRanges, from, to);
    if ((entry->type & MULTIBOOT_MEMORY_AVAILABLE) == 1) {
      AddToRange(fFreeRanges, from, to);
#if defined(DEBUG) && defined(DEBUG_BALLOC)
      kprintf("memory = {%x -> %x}\n", static_cast<uint32_t>(from),
              static_cast<uint32_t>(to));
#endif
    }

    ptr += entry->size + sizeof(entry->size);
  }

  /**
   * Just in case memory map is strange and contains overlapped
   * ranges we at first add all free ranges and then remove all
   * busy ranges. And we also must explicitly delete range occupied
   * by the kernel.
   **/
  ptr = begin;
  while (ptr + sizeof(multiboot_memory_map_t) <= end) {
    multiboot_memory_map_t* entry =
        reinterpret_cast<multiboot_memory_map_t*>(va(ptr));
    uint64_t from = entry->addr;
    uint64_t to = from + entry->len;
    if (entry->type != MULTIBOOT_MEMORY_AVAILABLE) {
      RemoveFromRange(fFreeRanges, from, to);
    }
    ptr += entry->size + sizeof(entry->size);
  }

  // defined in the linker script
  extern char text_phys_begin[];
  extern char bss_phys_end[];

  const uintptr_t kernel_begin = (uintptr_t)text_phys_begin;
  const uintptr_t kernel_end = (uintptr_t)bss_phys_end;

  RemoveFromRange(fFreeRanges, kernel_begin, kernel_end);

  // Drop the first 4Kb so that we can interpret 0 physical address as invalid
  AddToRange(fAllRanges, 0, 4096);
  RemoveFromRange(fFreeRanges, 0, 4096);
}

void Balloc::AddToRange(RangeVector& range, uint64_t start, uint64_t end) {
  uint64_t from = 0, to;
  while (from != range.size && start > range[from].end) {
    from++;
  }

  to = from;
  while (to != range.size && end >= range[to].begin) {
    to++;
  }

  if (range.size + 1 - (from - to) > BALLOC_MAX_RANGE_SZIE) {
    // there is no enough space
    // TODO support kernel panic
    while (1)
      ;
  }

  if (from != to) {
    if (start > range[from].begin) {
      start = range[from].begin;
    }

    if (end < range[to].end) {
      end = range[from].end;
    }
  }

  /**
   * Move free ranges starting from "to" position to "from + 1"
   * position. At position from will place our newly added range.
   **/
  memmove(range.ranges.begin() + from + 1, range.ranges.begin() + to,
          (range.size - to) * sizeof(Range));
  range.size += 1 - (to - from);
  range.ranges[from].begin = start;
  range.ranges[from].end = end;
}

void Balloc::RemoveFromRange(RangeVector& ranges, uint64_t begin,
                             uint64_t end) {
  size_t from = 0, to;
  while (from != ranges.size && begin > ranges[from].begin) {
    from++;
  }

  to = from;
  while (to != ranges.size && end >= ranges[to].begin) {
    to++;
  }

  if (from == to) {
    return;
  }

  const uint64_t before = ranges[from].begin;
  const uint64_t after = ranges[to - 1].end;

  memmove(ranges.ranges.begin() + from, ranges.ranges.begin() + to,
          (ranges.size - to) * sizeof(Range));
  ranges.size -= to - from;

  if (before < begin) {
    AddToRange(ranges, before, begin);
  }

  if (after > end) {
    AddToRange(ranges, end, after);
  }
}

uint64_t Balloc::AlignDown(uint64_t ptr, size_t align) {
  return ptr - ptr % align;
}

uint64_t Balloc::AlignUp(uint64_t ptr, size_t align) {
  return AlignDown(ptr + align - 1, align);
}
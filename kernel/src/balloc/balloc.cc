/*******************************************************************************
 * Project: moon-os                                                            *
 * File Created: Sunday, 8th November 2020 1:29:49 pm                          *
 * Author: tangruiwen (tangruiwen1989@gmail.com)                               *
 * Copyright - 2020                                                            *
 ******************************************************************************/
#include "balloc.hpp"

#include <moon/memory.h>
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

    AddRange(from, to);
    if ((entry->type & MULTIBOOT_MEMORY_AVAILABLE) == 1) {
      AddFreeRange(from, to);
#if defined(DEBUG) && defined(DEBUG_BALLOC)
      kprintf("memory = {%x -> %x}\n", static_cast<uint32_t>(from),
              static_cast<uint32_t>(to));
#endif
    }

    ptr += entry->size + sizeof(entry->size);
  }
}

void Balloc::AddRange(uint64_t from, uint64_t to) {}

void Balloc::AddFreeRange(uint64_t from, uint64_t to) {}
/*******************************************************************************
 * Project: moon-os                                                            *
 * File Created: Sunday, 8th November 2020 1:28:24 pm                          *
 * Author: tangruiwen (tangruiwen1989@gmail.com)                               *
 * Copyright - 2020                                                            *
 ******************************************************************************/

#include <multiboot/multiboot.h>
#include <stddef.h>

#include <array.hpp>

/**
 * @brief Bootstrap allocator
 *
 */
class Balloc final {
 public:
  /**
   * @brief Single instance pattern
   *
   * @return Balloc* Balloc instance
   */
  static Balloc* Instance();

  /**
   * @brief
   *
   * @param mb_info
   */
  void Init(multiboot_info_t* mb_info);

  /**
   * @brief Get the total memory this system have
   *
   * @return size_t memory size in bytes
   */
  size_t TotalSize();
  /**
   * @brief Get current available memory this system have
   *
   * @return size_t available memory size in bytes
   */
  size_t AvailableSize();

 private:
  struct Range {
    uint64_t begin = 0;
    uint64_t end = 0;

    Range() : begin(0), end(0) {}

    Range& operator=(const Range& other) = default;
  };
  Balloc() = default;
  ~Balloc() = default;

  void InitInternal();

  enum {
    BALLOC_MAX_RANGE_SZIE = 256,
  };

  struct RangeVector {
    algorithm::array<Range, BALLOC_MAX_RANGE_SZIE> ranges;
    uint32_t size = 0;

    RangeVector() = default;

    Range& operator[](uint32_t index) { return ranges[index]; }
    const Range& operator[](uint32_t index) const { return ranges[index]; }
  };

  uint64_t FindFreeRange(uint64_t from, uint64_t to, size_t size, size_t align);
  static void AddToRange(RangeVector& ranges, uint64_t begin, uint64_t end);
  static void RemoveFromRange(RangeVector& ranges, uint64_t begin,
                              uint64_t end);

  static uint64_t AlignDown(uint64_t ptr, size_t align);
  static uint64_t AlignUp(uint64_t ptr, size_t align);

  multiboot_info_t* fMultibootInfo;
  RangeVector fAllRanges;
  RangeVector fFreeRanges;
};
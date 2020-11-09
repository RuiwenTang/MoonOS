/*******************************************************************************
 * Project: moon-os                                                            *
 * File Created: Sunday, 8th November 2020 1:28:24 pm                          *
 * Author: tangruiwen (tangruiwen1989@gmail.com)                               *
 * Copyright - 2020                                                            *
 ******************************************************************************/

#include <multiboot/multiboot.h>
#include <stddef.h>

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
  /**
   * @brief Add memory range onto total range list
   *
   * @param from  begin address of available memory range
   * @param to    end address of available memory range
   */
  void AddRange(uint64_t from, uint64_t to);
  /**
   * @brief Add memory range onto free range list
   *
   * @param from  begin address of available memory range
   * @param to    end address of available memory range
   */
  void AddFreeRange(uint64_t from, uint64_t to);

 private:
  Balloc() = default;
  ~Balloc() = default;

  void InitInternal();

 private:
  struct Range {
    uint64_t begin = 0;
    uint64_t end = 0;

    Range() : begin(0), end(0) {}
  };

  enum {
    BALLOC_MAX_RANGE_SZIE = 256,
  };

  multiboot_info_t* fMultibootInfo;
  Range fAllRanges[BALLOC_MAX_RANGE_SZIE];
  Range fFreeRanges[BALLOC_MAX_RANGE_SZIE];
  size_t fCurrentRangSize = 0;
  size_t fCurrentFreeRangeSize = 0;
};
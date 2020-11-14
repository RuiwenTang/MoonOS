/*******************************************************************************
 * Project: moon-os                                                            *
 * File Created: Wednesday, 11th November 2020 11:59:44 am                     *
 * Author: tangruiwen (tangruiwen1989@gmail.com)                               *
 * Copyright - 2020                                                            *
 ******************************************************************************/

#pragma once

#include <stddef.h>
#include <stdint.h>

namespace algorithm {

template <typename T, size_t SIZE>
class array {
 public:
  using value_type = T;
  using iterator = T*;
  using const_iterator = const T*;
  using reference = T&;
  using const_reference = const T&;
  using size_type = size_t;

  // constructor
  array() = default;
  ~array() = default;

  // iterator support
  iterator begin() { return fData; }

  reference operator[](size_t index) { return fData[index]; }

  const_reference operator[](size_t index) const { return fData[index]; }

  size_type size() const { return fSize_; }

 private:
  T fData[SIZE];
  size_type fSize_ = 0;
};

}  // namespace algorithm
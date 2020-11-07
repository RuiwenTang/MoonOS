/*******************************************************************************
 * Project: moon-os                                                            *
 * File Created: Friday, 6th November 2020 11:37:22 pm                         *
 * Author: tangruiwen (tangruiwen1989@gmail.com)                               *
 * Copyright - 2020                                                            *
 ******************************************************************************/

#pragma once

namespace algorithm {

/**
 * @brief Base class for linked list
 * 
 * @tparam T 
 */
template <class T>
struct list {
  using self_type = list<T>;

  T data;
  self_type* next;
  self_type* prev;

  list() : data(), next(this), prev(this) {}
  virtual ~list() = default;

  bool empty() const {
    return next == this && prev == this;
  }
};
}  // namespace algorithm
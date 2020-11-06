/*******************************************************************************
 * Project: moon-os                                                            *
 * File Created: Friday, 6th November 2020 11:37:22 pm                         *
 * Author: tangruiwen (tangruiwen1989@gmail.com)                               *
 * Copyright - 2020 Your Company                                               *
 ******************************************************************************/
#pragma once

namespace algorithm {

template <class T>
struct list {
  using self_type = list<T>;

  T data;
  self_type* next;
  self_type* prev;

  list() : data(), next(nullptr), prev(nullptr) {}
  virtual ~list() = default;
};
}  // namespace algorithm
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
struct List {
  using self_type = List<T>;

  T data;
  self_type* next;
  self_type* prev;

  List() : data(), next(this), prev(this) {}
  List(const T& v) : data(v), next(this), prev(this) {}
  virtual ~List() = default;

  bool Empty() const { return next == this && prev == this; }

  void Add(self_type* node) { add_list_node(node, this, this->next); }

  void AddTail(self_type* node) { add_list_node(node, this->prev, this); }
  void Delete(self_type* node) {
    self_type* p = node->prev;
    self_type* n = node->next;

    p->next = n;
    n->prev = p;
  }

 private:
  static void add_list_node(self_type* node, self_type* prev, self_type* next) {
    node->prev = prev;
    node->next = next;
    prev->next = node;
    next->prev = node;
  }
};
}  // namespace algorithm
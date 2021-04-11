/*******************************************************************************
 * Project: moon-os                                                            *
 * File Created: Friday, 6th November 2020 11:37:22 pm                         *
 * Author: tangruiwen (tangruiwen1989@gmail.com)                               *
 * Copyright - 2020                                                            *
 ******************************************************************************/

#pragma once

#include <stddef.h>

namespace algorithm {

template <typename T>
struct ListNode {
  using self_type = ListNode<T>;
  self_type* next = nullptr;
  self_type* prev = nullptr;
  T obj;

  ListNode(T const& obj) : next{nullptr}, prev{nullptr}, obj{obj} {}
};

/**
 * @brief Base class for linked list
 *
 * @tparam T
 */
template <typename T>
class List {
  using node_type = ListNode<T>;

 public:
  List() : mFront{nullptr}, mBack{nullptr}, mNum{0} {}

  ~List() { clear(); }

  void clear() {
    node_type* node = mFront;
    while (node && node->next) {
      node_type* n = node->next;
      delete node;
      node = n;
    }

    mFront = nullptr;
    mBack = nullptr;
    mNum = 0;
  }

  void addBack(T const& obj) {
    node_type* node = new node_type(obj);

    if (!mFront) {
      mFront = node;
    } else if (mBack) {
      mBack->next = node;
      node->prev = mBack;
    }

    mBack = node;
    mNum++;
  }

  void addFront(T const& obj) {
    node_type* node = new node_type(obj);

    if (!mBack) {
      mBack = node;
    } else {
      mFront->prev = node;
      node->next = mFront;
    }
    mFront = node;
    mNum++;
  }

  void insert(T const& obj, size_t pos) {
    if (mNum == 0) {
      addBack(obj);
      return;
    }

    if (pos == 0) {
      addFront(obj);
      return;
    }

    node_type* current = mFront;
    for (size_t i = 0; i < pos && i < mNum && current->next != nullptr; i++) {
      current = current->next;
    }

    node_type* node = new node_type(obj);
    node->prev = current;
    node->next = current->next;

    current->next->prev = node;
    current->next = node;

    mNum++;
  }

  node_type* get(size_t pos) {
    if (pos >= mNum) {
      return nullptr;
    }

    node_type* current = mFront;

    for (size_t i = 0; i < pos && i < mNum && current->next != nullptr; i++) {
      current = current->next;
    }

    return current;
  }

  void replace(size_t pos, T obj) {
    if (pos >= mNum) {
      return;
    }

    node_type* current = mFront;
    for (size_t i = 0; i < pos; i++) {
      current = current->next;
    }

    current->obj = obj;
  }

  void remove(size_t pos) {
    if (pos >= mNum) {
      return;
    }
    node_type* current = mFront;

    for (size_t i = 0; i < pos; i++) {
      current = current->next;
    }

    removeNode(current);
  }

  void remove(T const& obj) {
    if (mNum == 0) {
      return;
    }

    node_type* current = mFront;
    while (current && current != mBack && current->obj != obj) {
      current = current->next;
    }

    if (current->obj == obj) {
      removeNode(current);
    }
  }

  size_t size() const { return mNum; }

  node_type* front() { return mFront; }

  node_type* back() { return mBack; }

 private:
  void removeNode(node_type* node) {
    if (mFront == node) {
      mFront = node->next;
    }
    if (mBack == node) {
      mBack = node->prev;
    }
    if (node->next) {
      node->next->prev = node->prev;
    }
    if (node->prev) {
      node->prev->next = node->next;
    }

    delete node;
    mNum--;

    if (mNum == 0) {
      mFront = mBack = nullptr;
    }
  }

 private:
  node_type* mFront;
  node_type* mBack;
  size_t mNum;
};

}  // namespace algorithm
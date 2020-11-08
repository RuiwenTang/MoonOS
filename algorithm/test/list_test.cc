/*******************************************************************************
 * Project: moon-os                                                            *
 * File Created: Saturday, 7th November 2020 12:11:35 am                       *
 * Author: tangruiwen (tangruiwen1989@gmail.com)                               *
 * Copyright - 2020                                                            *
 ******************************************************************************/
#include "list/list.hpp"

#include <iostream>

#include "gtest/gtest.h"

using IList = algorithm::List<int>;

TEST(list, list_empty) {
  IList list{};
  ASSERT_TRUE(list.Empty());
}

TEST(list, list_add) {
  IList list{1};
  IList list2{2};

  list.Add(&list2);

  ASSERT_TRUE(!list.Empty());
  ASSERT_EQ(list.next, &list2);

  IList list3{3};
  list.AddTail(&list3);
  ASSERT_EQ(list.prev, &list3);
  ASSERT_EQ(list3.next, &list);
}

TEST(list, list_delete) {
  IList list{1};
  IList list2{2};
  IList list3{3};

  list.Add(&list2);
  list.Add(&list3);

  ASSERT_EQ(list.next, &list3);
  list.Delete(&list3);
  ASSERT_EQ(list.next, &list2);
  list.Delete(&list2);
  ASSERT_TRUE(list.Empty());
}

TEST(list, list_splice) {
  IList list1{1};
  list1.Add(new IList(2));
  list1.Add(new IList(3));

  IList list2(10);
  list2.Add(new IList(20));
  list2.Add(new IList(30));
  list2.Add(new IList(40));

  list1.Splice(&list1, &list2);

  ASSERT_TRUE(list1.Empty());
  ASSERT_TRUE(list2.Empty());
}
/*******************************************************************************
 * Project: moon-os                                                            *
 * File Created: Saturday, 7th November 2020 12:11:35 am                       *
 * Author: tangruiwen (tangruiwen1989@gmail.com)                               *
 * Copyright - 2020                                                            *
 ******************************************************************************/
#include "list/list.hpp"

#include "gtest/gtest.h"

using IList = algorithm::List<int>;

TEST(List, create) {
  IList* list = new IList();
  EXPECT_NE(list, nullptr);
  EXPECT_EQ(list->size(), 0);
}

TEST(List, insert) {
  IList* list = new IList();
  EXPECT_EQ(list->size(), 0);

  list->addBack(1);
  EXPECT_EQ(list->size(), 1);
  list->addBack(2);
  EXPECT_EQ(list->size(), 2);
  list->addBack(2);
  EXPECT_EQ(list->size(), 3);

  EXPECT_EQ(1, list->front()->obj);
  EXPECT_EQ(2, list->back()->obj);
}
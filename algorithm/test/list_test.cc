/*******************************************************************************
 * Project: moon-os                                                            *
 * File Created: Saturday, 7th November 2020 12:11:35 am                       *
 * Author: tangruiwen (tangruiwen1989@gmail.com)                               *
 * Copyright - 2020                                                            *
 ******************************************************************************/
#include "list/list.hpp"

#include "gtest/gtest.h"

using IList = algorithm::list<int>;

TEST(list, list_empty) {
  IList list{};
  ASSERT_TRUE(list.empty());
}

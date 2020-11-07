#include "list/list.hpp"

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
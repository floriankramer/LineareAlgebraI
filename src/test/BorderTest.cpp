// Copyright 2016 Florian Kramer

#include <gtest/gtest.h>
#include "../Border.h"

TEST(Border, intersection) {
  ant::Border b1(0, 0, 100, 100);
  ant::Border b2(-100, 100, 100, -100);
  ASSERT_TRUE(b1.intersectsWith(&b2));
  ASSERT_TRUE(b2.intersectsWith(&b1));

  ant::Border b3(1, -100, 0, 1);
  ant::Border b4(-100, 0, 20, 0);
  ASSERT_TRUE(b3.intersectsWith(&b4));
  ASSERT_TRUE(b4.intersectsWith(&b3));

  ant::Border b5(0, 0, 100, 100);
  ant::Border b6(-100, -100, 100, -100);
  ASSERT_FALSE(b5.intersectsWith(&b6));
  ASSERT_FALSE(b6.intersectsWith(&b5));
}


TEST(Border, getIntersectionWith) {
  ant::Border b1(40, 1, 44, 5);
  ant::Border b2(40, 5, 44, 1);
  int *i = b1.getIntersectionWith(&b2);
  ASSERT_EQ(42, i[0]);
  ASSERT_EQ(3, i[1]);
  delete[] i;
}

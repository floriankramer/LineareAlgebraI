// Copyright 2016 Florian Kramer

#include <gtest/gtest.h>

#include <vector>
#include <cmath>

#include "Circle.h"

using ant::Circle;
using ant::CircleSlice;

TEST (CircleSlice, isContained) {
  CircleSlice c(0, 3, false);
  ASSERT_TRUE(c.isContained(2));
  ASSERT_FALSE(c.isContained(3.1));
  c.setCrossesZero(true);
  
  ASSERT_FALSE(c.isContained(2));
  ASSERT_TRUE(c.isContained(3.1));
}

TEST (Circle, operatorEq) {
  Circle c1(0, 0, 1);
  Circle c2(0, 0, 1);

  ASSERT_TRUE(c1 == c2);

  c2.setRadius(2);
  ASSERT_FALSE(c1 == c2);
  c2.setRadius(1);

  c2.setX(2);
  ASSERT_FALSE(c1 == c2);
  c2.setX(0);

  c2.setY(2);
  ASSERT_FALSE(c1 == c2);
  c2.setY(0);
}

/*TEST (Circle, randomIntersections) {
  float pi = 3.141592653589793;
  for (int i = 0; i < 100; i++) {
    float x1 = (rand() % 200) - 100;
    float y1 = (rand() % 200) - 100;
    float r1 = (rand() % 200) + 50;
    Circle c1(x1, y1, r1);
    Circle c2((rand() % 200) - 100, (rand() % 200) - 100, (rand() % 200) + 50);
    if (c2.contains(c2)) {
      i--;
      continue;
    }

    c1.computeIntersectionsWith(c2);

    float a1 = -42;
    float a2 = -42;

    float a = -pi;
    bool inside = false;
    int ct = 0;
    
    for (int i = 0; i < 10000; i++) {
      float x = x1 + cos(a) * r1; 
      float y = y1 + sin(a) * r1;
      bool b = c2.contains(x, y);
      if (i == 0) {
       if (b == true) {
        i--;
        ct ++;
        if (ct > 10000) {
          printf("unable to find collision free zone\n");
          break;
        }
       }
      } else {
        if (!inside && b) {
          a1 = a;
        } else if (inside && !b) {
          a2 = a;
        }
      }
      inside = b;
      a += 2 * pi / 10000;
      if (a > pi) {
        a -= 2 * pi;
      }
    }

    if (a1 != -42) {
      ASSERT_EQ(1, c1.getIntersections().size());
      CircleSlice c = c1.getIntersections()[0];
      if (a1 < a2) {
        if (!(fabs(c.getA1() - a1) < 0.01) || !(fabs(c.getA2() - a2) < 0.01)) {
          printf("%f - %f < 0.01 && %f - %f < 0.01\n", c.getA1(), a1, c.getA2(), a2);
        }
      } else { 
        if (!(fabs(c.getA1() - a2) < 0.01) || !(fabs(c.getA2() - a1) < 0.01)) {
          printf("%f - %f < 0.01 && %f - %f < 0.01\n", c.getA1(), a2, c.getA2(), a1);
        }
      }
    }
  }
}*/

TEST (Circle, intersections) {
  Circle c1(0, 0, 12);
  Circle c2(12, 0, 12);
  Circle c3(0, 12, 12);
  c1.computeIntersectionsWith(c2);
  c1.computeIntersectionsWith(c3);
  
  std::vector<CircleSlice> v = c1.getIntersections();
  ASSERT_EQ(2, v.size());
  CircleSlice s = v[0];

  ASSERT_FLOAT_EQ(-1.2897614, s.getA1());
  ASSERT_FLOAT_EQ(1.2897614, s.getA2());

  c1.unifyOverlappingIntersections();
  v = c1.getIntersections();

  ASSERT_EQ(1, v.size());
  s = v[0]; 
  ASSERT_FLOAT_EQ(-1.2897614, s.getA1());
  ASSERT_FLOAT_EQ(2.8605578, s.getA2());
}

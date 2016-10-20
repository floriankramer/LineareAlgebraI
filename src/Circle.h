// Copyright 2016 Florian Kramer

#ifndef ANT_CIRCLE_H_
#define ANT_CIRCLE_H_

#include <gtest/gtest_prod.h>

#include <vector>

namespace ant {

  class CircleSlice {
   public: 
      CircleSlice(float a1 = 0, float a2 = 0, bool crossesZero = false);

      bool isContained(float a) const;

      void setA1(float a1);
      void setA2(float a2);
      void setCrossesZero(bool b);

      float getA1() const;
      float getA2() const;
      bool isCrossesZero() const;

   private:
      float a1, a2;
      bool crossesZero;

  };

  class Circle {
   public:
      Circle(float x=0, float y=0, float radius=1);
      float getX() const;
      float getY() const;
      float getRadius() const;
      // computes intersections with c and stores the intersecing slice
      // in intersetcions
      void computeIntersectionsWith(const Circle &c);

      // merges all overlapping slices in intersections into
      // as large as possible slices
      void unifyOverlappingIntersections(); 

      bool contains(const Circle &c) const;
      bool contains(const float x, const float y) const;

      bool operator==(const Circle &c) const;
      bool operator!=(const Circle &c) const;

      std::vector<CircleSlice> getIntersections() const;

      void setRadius(const float r);
      void setX(const float x);
      void setY(const float y);

      unsigned int tesselate(float *f, const unsigned int numVertecies) const;

   private:
      float x, y, radius;
      std::vector<CircleSlice> intersections;
  };
}

#endif

// Copyright 2016 Florian Kramer

#define PI 3.141592653589793

#include "Circle.h"

#include <cmath>
#include <cstdio>
#include <algorithm>

namespace ant {

  // CircleSlice
  // ===========================================================================

  CircleSlice::CircleSlice(float a1, float a2, bool crossesZero) :
    a1(a1),
    a2(a2),
    crossesZero(crossesZero) {

    }

  bool CircleSlice::isContained(float a) const {
    if (crossesZero) {
      return a < a1 || a > a2;
    } else {
      return a > a1 && a < a2;
    }
  }

  void CircleSlice::setA1(float a1) {
    this->a1 = a1;
  }

  void CircleSlice::setA2(float a2) {
    this->a2 = a2;
  }

  void CircleSlice::setCrossesZero(bool b){
    this->crossesZero = b;
  }

  float CircleSlice::getA1() const {
    return a1;
  }

  float CircleSlice::getA2() const {
    return a2;
  }

  bool CircleSlice::isCrossesZero() const {
    return crossesZero;
  }


  // Circle
  // ===========================================================================

  Circle::Circle(float x, float y, float radius) :
    x(x),
    y(y),
    radius(radius) {

    }

  float Circle::getX() const {
    return x;
  }

  float Circle::getY() const {
    return y;
  }

  float Circle::getRadius() const {
    return radius;
  }

  void Circle::computeIntersectionsWith(const Circle &c) {

    float d = std::hypot(x - c.x, y - c.y);
    if (d < radius + c.radius) {
      // circles intersect
      CircleSlice is;
      // method as shown on http://stackoverflow.com/questions/3349125/circle-circle-intersection-points
      float a = (radius * radius - c.radius * c.radius + d * d) / (2 * d);
      //p printf("\n\na: %f\n", a);
      float h = sqrt(radius * radius - a * a);
      //p printf("h: %f\n", h);
      float mx = x + a * (c.x - x) / d;
      float my = y + a * (c.y - y) / d;
      //p printf("mx: %f\n", mx);
      //p printf("my: %f\n", my);

      float p1x = mx + h * (c.y - y) / d;
      float p1y = my - h * (c.x - x) / d;
      //p printf("p1x: %f\n", p1x);
      //p printf("p1y: %f\n", p1y);
      
      float p2x = mx - h * (c.y - y) / d;
      float p2y = my + h * (c.x - x) / d;
      //p printf("p2x: %f\n", p2x);
      //p printf("p2y: %f\n", p2y);

      is.setA1(atan2(p1y - y, p1x - x));
      is.setA2(atan2(p2y - y, p2x - x));
      if (is.getA1() > is.getA2()) {
        float t = is.getA2();
        is.setA2(is.getA1());
        is.setA1(t);
      }

      //p printf("a1: %f\n", is.getA1());
      //p printf("a2: %f\n", is.getA2());
      float ac = atan2(my - y, mx - x);
      // compute if the slice described crosses the zero mark
      is.setCrossesZero(ac < is.getA1() || ac > is.getA2());
      //p printf("cz: %d\n\n\n", is.isCrossesZero());

      intersections.push_back(is); 
    }

  }

  void Circle::unifyOverlappingIntersections() {
    for (unsigned int i = 0; i < intersections.size(); i++) {
      CircleSlice &is1 = intersections[i];
      for (unsigned int j = 0; j < intersections.size(); j++) {
        CircleSlice &is2 = intersections[j];
        if (j != i) {
          if (is1.isContained(is2.getA1()) && is1.isContained(is2.getA2())) {
            // printf("is2 is contained\n");
            // is2 contained in is1
            intersections.erase(intersections.begin() + j);
            if (i > j) {
              i--;
            }
            j--;
          } else if (is1.isContained(is2.getA1())) {

            // TODO depedning on whether the slice extends across zero the
            // coordinate which needs changing might be different (a1 vs a2) 

            // la1 is inside
            is1.setA2(is2.getA2());
            is1.setCrossesZero(is1.isCrossesZero() || is2.isCrossesZero()); 
            intersections.erase(intersections.begin() + j);
            if (i > j) {
              i--;
            }
            j = 0; // recheck all after size increase

          } else if (is1.isContained(is2.getA2())) {
            // printf("a2 is contained\n");
            // la2 is inside
            is1.setA1(is2.getA2());
            is1.setCrossesZero(is1.isCrossesZero() || is2.isCrossesZero());
            intersections.erase(intersections.begin() + j);
            if (i > j) {
              i--;
            }
            j = 0; // recheck all after size increase
          }
        }
      }
    }
    // sort slices by ascending lower bound
    std::sort(intersections.begin(), intersections.end(),
        [](const CircleSlice &c1, const CircleSlice &c2) {
        return (c1.isCrossesZero() ? c1.getA2() : c1.getA1()) < (c2.isCrossesZero() ? c2.getA2() : c2.getA1());
        });
  }

  unsigned int Circle::tesselate(float *f, const unsigned int numVertecies) const {
    float sliceSize = PI / numVertecies * 4;
    float a = -PI;
    unsigned int u = 0;
    for (unsigned int i = 0; i < numVertecies / 2; i++) {
      bool b = false;
      for (const CircleSlice &c : intersections) { 
        if (c.isContained(a) || c.isContained(a + sliceSize)) {
          b = true;
          break;
        }
      }
      if (!b) {
        f[u * 4 + 0] = x + cos(a) * radius;
        f[u * 4 + 1] = y + sin(a) * radius;
        a += sliceSize;
        f[u * 4 + 2] = x + cos(a) * radius;
        f[u * 4 + 3] = y + sin(a) * radius;
        u ++;
      } else{
        a += sliceSize;

      }
    }

    return u * 2;

    unsigned int numV = 0;
    // number of vertecies to be spread evenly acros the circle
    // unsigned int numNormalV = numVertecies - intersections.size() * 2;
    //float a = 0;
   
    unsigned int currentSlice = 0;
    int i = 0;

    if (intersections.size() > 0) {
      const CircleSlice &s = intersections[0];
      if (s.isCrossesZero()) {
        a = s.getA2();
      } else {
        a = s.getA1();
      }
      if (std::isnan(a)) {
        printf("isnan %d\n", s.isCrossesZero());
      } else {
        printf("nonan %d\n", s.isCrossesZero());
      }
      if (a < 0) {
        a += 2 * PI;
      }
      f[numV * 2 + 0] = x + cos(a) * radius;
      f[numV * 2 + 1] = y + sin(a) * radius;
      printf("a1: %f\n", a);
      i = a / sliceSize + 1;
      a = i * sliceSize; 
      printf("a2: %f\n\n", a);
      f[numV * 2 + 2] = x + cos(a) * radius;
      f[numV * 2 + 3] = y + sin(a) * radius; 
      i++;
      numV += 2;
      currentSlice++;
      currentSlice %= intersections.size();
    }

    while (false && numV < numVertecies) {
      f[numV * 2 + 0] = x + cos(a) * radius;
      f[numV * 2 + 1] = y + sin(a) * radius;
      a += sliceSize;
      if (intersections.size() > 0) {
        if (intersections[currentSlice].getA1() < a) {
          a = intersections[currentSlice].getA1();
          f[numV * 2 + 2] = x + cos(a) * radius;
          f[numV * 2 + 3] = y + sin(a) * radius;
          i++;
          numV += 2;
          if (currentSlice == 0) break;
          a = intersections[currentSlice].getA2();
          f[numV * 2 + 0] = x + cos(a) * radius;
          f[numV * 2 + 1] = y + sin(a) * radius;
          i = a / sliceSize + 1;
          a = i * sliceSize;
        }
      }
      f[numV * 2 + 2] = x + cos(a) * radius;
      f[numV * 2 + 3] = y + sin(a) * radius;
      i++;
      numV += 2;
    } 
    return numV;
  }

  bool Circle::contains(const Circle &c) const {
    return hypot(x - c.x, y - c.y) < radius - c.radius;
  }

  bool Circle::contains(const float x, const float y) const {
    return hypot(this->x - x, this->y - y) < radius;
  }

  bool Circle::operator==(const Circle &c) const {
    return c.x == x && c.y == y && c.radius == radius;
  }

   bool Circle::operator!=(const Circle &c) const { 
    return !operator==(c); 
  }

  std::vector<CircleSlice> Circle::getIntersections() const {
    return intersections;
  }

  void Circle::setRadius(const float r) {
    radius = r;
  }

  void Circle::setX(const float x) {
    this->x = x;
  }

  void Circle::setY(const float y) {
    this->y = y;
  }
}

// Copyright 2016 Florian Kramer

#include "MetaBall.h"

#include <cmath>

MetaBall::MetaBall(float threshold, float gooiness) :
  threshold(threshold),
  gooiness(gooiness) {

  }

MetaBall::~MetaBall() {

}

void MetaBall::addMetaBall(const float x, const float y, const float size) {
  MBall m;
  m.x = x;
  m.y = y;
  m.size = size;
  mBalls.push_back(m);
}


float MetaBall::value(const float x, const float y) const {
  float v = 0;
  for (const MBall &m : mBalls) {
    v += m.size / (std::pow(std::hypot(m.x - x, m.y - y), gooiness));
  }
  return v; 
}

bool MetaBall::inside(const float x, const float y) const {
  return value(x, y) > threshold;
}

std::vector<std::array<float, 4>> MetaBall::tesselate(const float resolution) const {
  std::vector<std::array<float, 4>> points;

  if (mBalls.size() == 0) {
    return points;
  }

  // compute grid size
  const MBall &m1 = mBalls[0]; 
  float minX = m1.x - m1.size;
  float maxX = m1.x + m1.size;
  float minY = m1.y - m1.size;
  float maxY = m1.y + m1.size;
  for (unsigned int i = 1; i < mBalls.size(); i++) {
    const MBall m = mBalls[i];
    if (m.x - m.size < minX) {
      minX = m.x - m.size;
    }
    if (m.x + m.size > maxX) {
      maxX = m.x + m.size;
    }
    if (m.y - m.size < minY) {
      minY = m.y - m.size;
    }
    if (m.y + m.size > maxY) {
      maxY = m.y + m.size;
    }
  }

  // seems to lead to odd cutoffs
/*  minX = std::floor(minX / resolution) * resolution;
  maxX = std::ceil(maxX / resolution) * resolution;
  minY = std::floor(minY / resolution) * resolution;
  maxY = std::ceil(maxY / resolution) * resolution;*/

  // debug adding bounding box for grid
   /* points.push_back(std::array<float, 4> {{
      minX,
      minY,
      maxX,
      minY }});
  points.push_back(std::array<float, 4> {{
      maxX,
      minY,
      maxX,
      maxY }});  
  points.push_back(std::array<float, 4> {{
      minX,
      maxY,
      maxX,
      maxY }});  
  points.push_back(std::array<float, 4> {{
      minX,
      minY,
      minX,
      maxY }}); */

  unsigned int width = std::lround((maxX - minX) / resolution) + 1;
  unsigned int height = std::lround((maxY - minY) / resolution) + 1;
  // allocate space for grid
  float *grid = new float[width * height]; 
  float v;

  // compute grid corner values
  for (unsigned int x = 0; x < width; x++) {
    for (unsigned int y = 0; y < height; y++) {
      v = value(minX + x * resolution - resolution / 2, minY + y * resolution - resolution / 2);
      grid[y + height * x] = v > threshold ? v : 0; 
    }
  }
  
  // based upon grid corner wheights, add lines
  for (unsigned int x = 0; x < width - 1; x++) {
    for (unsigned int y = 0; y < height - 1; y++) {
      int i = 0;
      i += grid[y + 1 + height * x] > threshold * 0.5 ? 1 : 0;  // upper left
      i += grid[y + 1 + height * (x + 1)] > threshold * 0.5 ? 2 : 0;  // upper right
      i += grid[y + height * x] > threshold * 0.5 ? 4 : 0;  // lower left
      i += grid[y + height * (x + 1)] > threshold * 0.5 ? 8 : 0;  // lower right
      switch(i) {
        case 0:  // no corners inside meta ball
          break;
        case 1:  // ul
        case 14:  // lr + ll + ur
          points.push_back(std::array<float, 4> {{
              minX + x * resolution,
              minY + (y + 0.5f) * resolution,
              minX + (x + 0.5f) * resolution,
              minY + (y + 1) * resolution}});
          break;
        case 2:  // ur
        case 13:  // lr + ll + ul
          points.push_back(std::array<float, 4> {{
              minX + (x + 0.5f) * resolution,
              minY + (y + 1) * resolution,
              minX + (x + 1) * resolution,
              minY + (y + 0.5f) * resolution}});
          break;
        case 3:  // ul + ur
        case 12:  // lr + ll
          points.push_back(std::array<float, 4> {{
              minX + x * resolution,
              minY + (y + 0.5f) * resolution,
              minX + (x + 1) * resolution,
              minY + (y + 0.5f) * resolution}});
          break;
        case 4:  // ll
        case 11:  // lr + ur + ul
          points.push_back(std::array<float, 4> {{
              minX + x * resolution,
              minY + (y + 0.5f) * resolution,
              minX + (x + 0.5f) * resolution,
              minY + (y) * resolution}});
          break;
        case 5:  // ll + ul
        case 10:  // lr + ur
          points.push_back(std::array<float, 4> {{
              minX + (x + 0.5f) * resolution,
              minY + (y) * resolution,
              minX + (x + 0.5f) * resolution,
              minY + (y + 1) * resolution}});
          break;
        case 6:  // ll + ur 
          break;
        case 7:  // ll + ur + ul
          points.push_back(std::array<float, 4> {{
              minX + (x + 0.5f) * resolution,
              minY + (y) * resolution,
              minX + (x + 1) * resolution,
              minY + (y + 0.5f) * resolution}});
          break;
        case 8:  // lr
          points.push_back(std::array<float, 4> {{
              minX + (x + 0.5f) * resolution,
              minY + (y) * resolution,
              minX + (x + 1) * resolution,
              minY + (y + 0.5f) * resolution}});
          break;
        case 9:  // lr + ul
          break;
        case 15:  // all corners inside meta ball
          break;
      }
    }
  } 

  
  return points;
}

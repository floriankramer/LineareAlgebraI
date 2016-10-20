// Copyright 2016 Florian Kramer

#ifndef ANT_METABALL_H_
#define ANT_METABALL_H_

#include <vector>
#include <array>

struct MBall {
  float x, y;
  float size;
};

class MetaBall {
 public:
    MetaBall(float threshold = 0.1, float gooiness = 1.9);
    virtual ~MetaBall();
    void addMetaBall(const float x, const float y, const float size);
    std::vector<std::array<float, 4>> tesselate(const float resolution = 25) const;
    bool inside(const float x, const float y) const;
    float value(const float x, const float y) const;

 private:
  float threshold, gooiness;
  std::vector<MBall> mBalls;

};

#endif  // ANT_METABALL_H_

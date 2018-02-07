// Copyright 2016 Florian Kramer

#include "PerlinNoise.h"

#include <cmath>
#include <ctime>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <cstring>

const unsigned char staticPerm[] = {
  151,160,137,91,90,15, 131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
  8,99,37,240,21,10,23,190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
  35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,
  134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
  55,46,245,40,244,102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,
  18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,
  226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,
  17,182,189,28,42,223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,
  155,167, 43,172,9,129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,
  104,218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,
  235,249,14,239,107,49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,
  45,127, 4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,
  215,61,156,180,

  151,160,137,91,90,15, 131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
  8,99,37,240,21,10,23,190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
  35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,
  134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
  55,46,245,40,244,102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,
  18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,
  226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,
  17,182,189,28,42,223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,
  155,167, 43,172,9,129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,
  104,218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,
  235,249,14,239,107,49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,
  45,127, 4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,
  215,61,156,180
};

PerlinNoise::PerlinNoise(int seed) {
  if (seed < 0) {
    this->seed = time(NULL);
  } else {
    this->seed = seed;
  }
  unsigned int s = this->seed;
  permutation = new unsigned char[512];
  std::vector<unsigned char> v;
  for (unsigned int i = 0; i < 256; i++) {
    v.push_back(i);
  }
  for (unsigned int p = 0; p < 256; p++) {
    unsigned int i = rand_r(&s) % v.size();
    permutation[p] = v[i];
    permutation[p + 256] = v[i];
    v.erase(v.begin() + i); 
  }
}
PerlinNoise::~PerlinNoise() {
  delete[] permutation;
}

double PerlinNoise::value(double x, double y) {
  // TODO handle cases x > 255 || y > 255
  int bX = static_cast<int>(std::floor(x));
  int bY = static_cast<int>(std::floor(y));
  double dx = x - bX;
  double dy = y - bY;
  double u = smootherstep(dx);
  double v = smootherstep(dy);
  // hash coordinates
  double ll = permutation[permutation[bX    ] + bY    ];
  double lr = permutation[permutation[bX + 1] + bY    ];
  double ul = permutation[permutation[bX    ] + bY + 1];
  double ur = permutation[permutation[bX + 1] + bY + 1];

  double g1 = grad(ll, dx    , dy    ); 
  double g2 = grad(lr, dx - 1, dy    );
  double g3 = grad(ul, dx    , dy - 1);
  double g4 = grad(ur, dx - 1, dy - 1);

  double v1 = lerp(g1, g2, u); 
  double v2 = lerp(g3, g4, u);

  return (lerp(v1, v2, v) + 1) / 2;
}

double PerlinNoise::grad(int h, double x, double y) {
  switch (h & 3) {
    case 0:
      return x + y;
    case 1:
      return -x + y;
    case 2:
      return x - y;
    case 3:
      return -x - y;
    default:
      return 0;
  } 
}

double PerlinNoise::lerp(double a, double b, double x) {
  return a + x * (b - a);
}

double PerlinNoise::smootherstep(double x) {
  // 6 x^5 - 15 x^4 + 10 x^3
  return x * x * x * (x * (x * 6 - 15) + 10);
}

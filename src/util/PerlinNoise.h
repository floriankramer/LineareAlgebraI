// Copyright 2016 Florian Kramer

#ifndef ANT_PERLIN_NOISE_H_
#define ANT_PERLIN_NOISE_H_

class PerlinNoise {
 public:
    PerlinNoise(int seed = -1);
    virtual ~PerlinNoise();
    double value(double x, double y);

 private:
    // smootherstep as descibed on https://en.wikipedia.org/wiki/Smoothstep
    double smootherstep(double x);
    double grad(int h, double x, double y);
    double lerp(double a, double b, double x);
    unsigned int seed;
    unsigned char *permutation;
};

#endif  // ANT_PERLIN_NOISE_H_

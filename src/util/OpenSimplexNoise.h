// Copyright 2016 Florian Kramer

#ifndef ANT_OPENSIMPLEXNOISE_H_
#define ANT_OPENSIMPLEXNOISE_H_

class OpenSimplexNoise {
  public:
     OpenSimplexNoise(int seed = -1);
     virtual ~OpenSimplexNoise();

     double value(double x, double y);

  private:
     double extrapolate(int xsb, int ysb, double dx, double dy);
     int fastFloor(double x);
     unsigned char *perm;
};

#endif  // ANT_OPENSIMPLEXNOISE_H_

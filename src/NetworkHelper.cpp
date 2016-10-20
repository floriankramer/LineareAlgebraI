// Copyright 2016 Florian Kramer

#include "NetworkHelper.h"

#include "warp/Network.h"

namespace ant{

  void writeIntToBuffer(int value, unsigned char *buffer, unsigned int offset){
    warp::writeIntToNetworkBuffer(value, buffer, offset);
  }

  int readIntFromBuffer(unsigned char *buffer, unsigned int offset){
    return warp::readIntFromNetworkBuffer(buffer, offset);
  }

  void writeFloatToBuffer(float value, unsigned char *buffer, unsigned int offset){
    warp::writeFloatToNetworkBuffer(value, buffer, offset);
  }

  float readFloatFromBuffer(unsigned char *buffer, unsigned int offset){
    return warp::readFloatFromNetworkBuffer(buffer, offset);
  }

}

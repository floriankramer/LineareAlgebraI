// Copyright 2016 Florian Kramer

#define NETWORK_BUFF_SIZE 1024

struct ConnectionBuffer;

namespace warp{
  int sendPacketTo(int socket, const unsigned char *data, unsigned int length);
  int readPackets(int socket, ConnectionBuffer *buff, void (*handler)(ConnectionBuffer*, void *), unsigned char *buffer,  void *userdata = nullptr);
}

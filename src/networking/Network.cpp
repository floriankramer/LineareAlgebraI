// Copyright 2016 Florian Kramer

#include "Network.h"

#ifdef unix
#include <sys/socket.h>
#include <netinet/in.h>
#include <ifaddrs.h>
// allows for setting sockets asynchronious
#include <fcntl.h>
#include <errno.h>
// provides close
#include <unistd.h>
// provides inet_aton
#include <arpa/inet.h>
#include <thread>
#endif


#ifdef windows
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <mingw.thread.h>
#endif


#include <cstring>
#include <vector>
#include <map>

#include "general/os.h"
#include "NetworkHelper.h"
#include "general/Logger.h"

namespace warp{
  Logger networkLogger("Network");

  int readPackets(int socket, ConnectionBuffer *buff, void (*handler)(ConnectionBuffer*, void *), unsigned char *buffer,  void *userdata) {
    if (socket < 3) {
      networkLogger.err("Attempted to read pckets from socket < 3");
      return 0;
    }
    int numRead = 1;
    while(numRead > -1) {
#ifdef unix
      numRead = read(socket, buffer, NETWORK_BUFF_SIZE);
      unsigned int off = 0;
      unsigned int l = 0;
      // networkLogger.log("read %d bytes from stream\n", numRead);
      // networkLogger.log("read data from stream");
      if(numRead > -1) {
        //        printPacket("received data: ", buffer, numRead);  
        while(off < static_cast<unsigned int>(numRead)) {
          // networkLogger.log("processed %d of %d bytes from the current buffer\n", off, numRead);
          if(buff->packetLength == 0) {
            buff->packetLength = buffer[off]; 
            buff->dataLength = buff->packetLength;
            off += 1;
          }

          if(numRead - off < buff->packetLength - buff->bytesReceived) {
            l = numRead - off;
          }
          else{
            l = buff->packetLength - buff->bytesReceived; 
          }

          memcpy(buff->buffer + buff->bytesReceived, buffer + off, l);
          buff->bytesReceived += l;
          off += l;

          if(buff->bytesReceived == buff->packetLength) { 
            // networkLogger.log("Read full packet: \n");
            (*handler)(buff, userdata);
            buff->packetLength = 0;
            buff->dataLength = 0;
            buff->bytesReceived = 0;
          }
        }

      }
      else{
        // TODO Florian: Handle -1 returned by read call on socket
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
          char buf[1024];
          strerror_r(errno, buf, 1023);
          buf[1023] = 0;
          networkLogger.err("ERROR on read from socket %s %s:%d", strerror_r(errno, buf, 1023), __FILE__, __LINE__);
        }
      }
#endif
#ifdef windows

#endif
    }
    if(numRead < 0) {
      return numRead;
    }
    return 0;
  }

  int sendPacketTo(int socket, const unsigned char *data, unsigned int length) { 
    if (socket < 3) {
      networkLogger.err("tried writing packet to socket with id < 3");
      return 0;
    }
    unsigned char *packet = new unsigned char[1 + length];
    packet[0] = length;  // set byte 0 to length of data

    std::memcpy(packet + 1, data, length); // TODO think about using two write commands, first writing array of size 1, then data
    int toWrite = length + 1;
    int written = 0;
#ifdef unix
    // used for timeouts
    fd_set fdset; 
    FD_ZERO(&fdset);
    FD_SET(socket, &fdset);

    timeval t;
    t.tv_sec = 60;
    t.tv_usec = 0;

    while(toWrite > 0) {
      written = write(socket, packet, toWrite);
      if(written > -1) {
        toWrite -= written;
      }
      else{
        if(errno == EWOULDBLOCK || errno == EAGAIN) {
          networkLogger.log("waiting for buffer to empty\n");
          select(1, NULL, &fdset, NULL, &t);// wait for up to a minute to send the packet, should the buffer be full
        }
        else{
          networkLogger.log("NETWORKING ERROR while writing: ");
          //networkLogger.log(strerror(errno));
          networkLogger.log("\n");
          return -1;
        }
      }
    } 
#endif
#ifdef windows

#endif
    delete[] packet;
    return 0;
  }  

  void writeIntToNetworkBuffer(int value, unsigned char *buffer, unsigned int offset) {
    bool sign = value >= 0; 
    uint32_t v = std::abs(value);
    if(sign) {
      v &= ~(1 << 31);
    }
    else{
      v |= 1 << 31;
    }
    std::memcpy(buffer + offset, &v, 4);
  }

  int readIntFromNetworkBuffer(unsigned char *buffer, unsigned int offset) {
    uint32_t v;
    std::memcpy(&v, buffer + offset, 4);

    bool sign = ((v >> 31) & 1) == 0;
    int value = v & ~(1<<31);
    if(!sign) {
      value = -value;
    }
    return value;
  }

  void writeFloatToNetworkBuffer(float value, unsigned char *buffer, unsigned int offset) {
    writeIntToNetworkBuffer(static_cast<int>(value * 2000), buffer, offset);
  }

  float readFloatFromNetworkBuffer(unsigned char *buffer, unsigned int offset) {
    return readIntFromNetworkBuffer(buffer, offset) / 2000.0;
  }
}

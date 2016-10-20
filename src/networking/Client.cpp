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
#include "general/Logger.h"

// internal network functions and definitions
#include "NetworkHelper.h"

namespace warp {
  namespace client {
    unsigned char utilBuffer[NETWORK_BUFF_SIZE];
    int cnt = -1;

#ifdef unix
    sockaddr_in serverAddr;
#endif
#ifdef windows
    WSADATA wsaData;
#endif

    ConnectionBuffer connectionBuffer;
    unsigned char buffer[NETWORK_BUFF_SIZE];
    void (*clientPacketCallback)(ConnectionBuffer *conBuff);

    void clientPacketCallbackWrapper(ConnectionBuffer *conBuff, void *data);

    bool startClient(std::string serverIp, unsigned int port) {
#ifdef unix
      serverAddr.sin_family = AF_INET;
      serverAddr.sin_port = htons(port);
      inet_aton(serverIp.c_str(), &serverAddr.sin_addr);  

      cnt = socket(AF_INET, SOCK_STREAM, 0);
      if(cnt == -1) {
        networkLogger.err("CLIENT ERROR while trying to create socket\n");
        return false;
      }

      if(connect(cnt, (sockaddr *)(&serverAddr), sizeof(serverAddr)) == -1) {
        close(cnt);
        networkLogger.err("CLIENT ERROR while trying to connect to ip %s on port 25542", serverIp.c_str());
        cnt = -1;
        return false;
      }
      int flags = fcntl(cnt, F_GETFL, 0);
      flags = flags | O_NONBLOCK;
      fcntl(cnt, F_SETFL, flags);
#endif
#ifdef windows

#endif

      return true;
    }

    void stopClient() {
      if(cnt != -1) {
#ifdef unix
        close(cnt);
#endif
#ifdef windows

#endif
        cnt = -1;
      }
    }

    void update() { 
      warp::readPackets(cnt, &connectionBuffer, clientPacketCallbackWrapper, buffer);
    }

    void setClientPacketCallback(void (*callback)(ConnectionBuffer *conBuff)) {
      clientPacketCallback = callback;
    }

    void sendPacket(unsigned char *data, unsigned int length) {
      if(cnt != -1) { 
        sendPacketTo(cnt, data, length);
      }
    }

    void clientPacketCallbackWrapper(ConnectionBuffer *conBuff, void *data) {
      clientPacketCallback(conBuff);
    }

    bool isConnected() {
      return cnt != -1;
    }
  }  // namespace client
}  // namespace warp

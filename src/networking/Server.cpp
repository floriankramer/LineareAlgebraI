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
  namespace server{
    void run();
    std::thread *connectionThread;
    bool shouldRun = false;
    unsigned char playerIdCounter = 1;
    int socketId = -1;

#ifdef unix
    sockaddr_in addr;
#endif
#ifdef windows
    WSADATA wsaData;
#endif

    std::vector<std::pair<unsigned int, int>> clients; //pairs of cid and socket
    std::map<unsigned int, int> clientMap;
    std::map<int, ConnectionBuffer> packetBuffer;    
    unsigned char buffer[NETWORK_BUFF_SIZE];

    void (*serverPacketCallback)(ConnectionBuffer *conBuff, int clientId);
    void (*serverConnectionCallback)(unsigned int clientId);

    void serverPacketCallbackWrapper(ConnectionBuffer *conBuff, void *data);

    void startServer(int port) {
#ifdef unix
      addr.sin_family = AF_INET;
      addr.sin_port = htons(port);  
      addr.sin_addr.s_addr = INADDR_ANY;

      socketId = socket(AF_INET, SOCK_STREAM, 0);
      if(socketId == -1) {
        networkLogger.err("SERVER ERROR: unable to open socket\n");

      }
      if(bind(socketId, (sockaddr*)&addr, sizeof(sockaddr_in)) == -1) {
        networkLogger.err("SERVER ERROR: unable to bind socket\n");
      }
      if(listen(socketId, 16) == -1) {
        networkLogger.err("SERVER ERROR: unable to set socket to listen\n");
      }
#endif
      shouldRun = true;
      connectionThread = new std::thread(run);
    }


    void stopServer() {
#ifdef unix
      if(socketId != -1) {
        for(auto a : clients) {
          shutdown(a.second, SHUT_RDWR);
          close(a.second);
        }
        shutdown(socketId, SHUT_RDWR);
        close(socketId);
      }
#endif
      shouldRun = false;
    }


    void run() {
      while(shouldRun) {
#ifdef unix
        sockaddr clientAddr;
        socklen_t clientAddrSize = sizeof(clientAddr);

        int cSoc = accept(socketId, &clientAddr, &clientAddrSize);
        if (!shouldRun) break;
        if(cSoc == -1) {
          networkLogger.log("SERVER ERROR: unable to accept client connection\n");
        }
        else{
          networkLogger.log("SERVER: new client connected\n");

          unsigned char cId = playerIdCounter;
          playerIdCounter ++;

          clients.push_back(std::pair<int, int>(cId, cSoc));  
          clientMap[cId] = cSoc;
          packetBuffer[cId] = ConnectionBuffer();
          memset(packetBuffer[cId].buffer, 0, 256);
          serverConnectionCallback(cId);

          //set socket to nonblocking
          int flags = fcntl(cSoc, F_GETFL, 0);
          if(flags < 0) {
            networkLogger.log("ERROR while querying flags for client socket\n");
          }
          else{
            flags = flags | O_NONBLOCK;
            fcntl(cSoc, F_SETFL, flags);
          }
        } 
#endif
#ifdef windows

#endif
      }
    }


    void readServerPackets() {
      std::vector<std::pair<int, int>> disconnectedClients;
      for(auto a : clients) {
        if(warp::readPackets(a.second, &packetBuffer[a.first], serverPacketCallbackWrapper, buffer, &a.first) < 0) {
#ifdef unix
          if(errno == ECONNABORTED || errno == ECONNRESET) {
            disconnectedClients.push_back(a);
          }
          else if(errno != EWOULDBLOCK) {
            char buf[1024];
            strerror_r(errno, buf, 1023);
            buf[1023] = 0;  // print at most 1024 chars, should the string written have no terminating 0
            networkLogger.log("SERVER ERROR on read: %s", buf);  
          }
#endif
        }
      }

      for(auto a : disconnectedClients) {
        disconnectClient(a.first);    
      }

    }

    void setServerPacketCallback(void (*callback)(ConnectionBuffer *conBuff, int clientId)) {
      serverPacketCallback = callback;
    }


    void sendPacket(int clientId, unsigned char *data, unsigned int length) {
      int cnt = clientMap[clientId];
      if(cnt != -1) {
        sendPacketTo(cnt, data, length);
      }
    }


    void broadcastPacket(unsigned char *data, unsigned int length) {
      for(auto a : clients) {
        sendPacketTo(a.second, data, length);
      }

    }


    void setClientConnectedCallback(void (*callback)(unsigned int clientId)) {
      serverConnectionCallback = callback;
    }

    void disconnectClient(unsigned int clientId) {
      int socId = clientMap[clientId];
#ifdef unix
      close(socId);
#endif
      packetBuffer.erase(clientId);
      clientMap.erase(clientMap.find(clientId));
      for(auto it = clients.begin(); it != clients.end(); it++) {
        if(it->first == clientId) {
          clients.erase(it);
          break;
        }
      } 
    }

    void serverPacketCallbackWrapper(ConnectionBuffer *conBuff, void *data) {
      serverPacketCallback(conBuff, *reinterpret_cast<int*>(data));
    }
  }  // namespace server

}


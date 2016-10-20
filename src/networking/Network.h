#ifndef WARP_NETWORK_H
#define WARP_NETWORK_H

#include <string>




namespace warp {
  // forward declarations
  class Logger;

  struct ConnectionBuffer {
    unsigned char buffer[256];
    unsigned int packetLength = 0;
    unsigned int bytesReceived = 0;
    unsigned int dataLength = 0;  
  };

  namespace server{
    void startServer(int port);
    void stopServer();


    //set function called, whenever the server receives a packet
    void setServerPacketCallback(void (*callback)(ConnectionBuffer *conBuff, int clientId));

    void sendPacket(int clientId, unsigned char *data, unsigned int length);
    void broadcastPacket(unsigned char *data, unsigned int length);
    void setClientConnectedCallback(void (*callback)(unsigned int clientId));

    void disconnectClient(unsigned int clientId);
    void readServerPackets();
  }  // namespace server

  namespace client{
    bool startClient(std::string serverIp, unsigned int port);
    void stopClient();
    void setClientPacketCallback(void (*callback)(ConnectionBuffer *conBuff));
    void sendPacket(unsigned char *data, unsigned int length);
    void update();
    bool isConnected();
  }  // namespace client

  void writeIntToNetworkBuffer(int value, unsigned char *buffer, unsigned int offset=0);
  int readIntFromNetworkBuffer(unsigned char *buffer, unsigned int offset=0);

  void writeFloatToNetworkBuffer(float value, unsigned char *buffer, unsigned int offset=0);
  float readFloatFromNetworkBuffer(unsigned char *buffer, unsigned int offset=0);

  extern Logger networkLogger;
}  // namespace warp

#endif

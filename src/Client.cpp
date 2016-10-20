#include "Client.h"
#include "Packets.h"
#include <cstring>

#ifdef unix
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <unistd.h>
#endif
#include <fcntl.h>

#include "WorldClient.h"
#include "NodeResource.h"
#include "NodeHive.h"
#include "NodeTerritory.h"
#include "warp/Network.h"

namespace ant{
  namespace client{ 
    sockaddr_in serverAddr;
    void handlePacket(warp::ConnectionBuffer *buff);

    warp::ConnectionBuffer connectionBuffer;
    unsigned char buffer[256];

    void sendPacket(PACKET_TYPE type, const unsigned char *data, unsigned int length){
      if(warp::client::isConnected()) { 
        unsigned char *c = new unsigned char[length + 1];
        c[0] = type;
        std::memcpy(c + 1, data, length);
        warp::client::sendPacket(c, length + 1);
        delete[] c;
      }
    }

    void sendPacket(PACKET_TYPE type, const char *data, unsigned int length){
      if(warp::client::isConnected()){
        unsigned char *temp = new unsigned char[length + 1];
        temp[0] = type;
        memcpy(temp + 1, data, length);
        warp::client::sendPacket(temp, length + 1);
        delete[] temp;
      }
    }

    bool connectTo(std::string ip, std::string username, int timeout){
      warp::client::setClientPacketCallback(handlePacket);

      warp::client::startClient(ip, 25542); 
      sendPacket(USERNAME, username.c_str(), username.length() + 1);
      return true;
    }

    void stop(){
      unsigned char d[1] = {static_cast<unsigned char>(world::getPlayerId())};
      sendPacket(PLAYER_LEFT, d, 1);
      warp::client::stopClient();
    }

    void handlePacket(warp::ConnectionBuffer *buff){
      PACKET_TYPE t = static_cast<PACKET_TYPE>(buff->buffer[0]);
      //printf("Client got packet of type %d\n", t);
      switch(t){
        case ID_ASSIGNMENT:
          printf("was assigned id %d by server\n", buff->buffer[1]);
          world::setPlayerId(buff->buffer[1]);
          break;
        case PLAYER_JOINED:

          break;
        case PLAYER_LEFT:

          break;
        case NEW_NODE_RESSOURCE: 
          world::addNode(createNodeResourceFromPacket(buff->buffer + 1));
          break;
        case NEW_NODE_TERRITORY:
          world::addNode(createNodeTerritoryFromPacket(buff->buffer + 1));
          break;

        case NEW_BORDER:
          { 
            int d[5] = {warp::readIntFromNetworkBuffer(buff->buffer, 1), warp::readIntFromNetworkBuffer(buff->buffer, 5), warp::readIntFromNetworkBuffer(buff->buffer, 9), warp::readIntFromNetworkBuffer(buff->buffer, 13), warp::readIntFromNetworkBuffer(buff->buffer, 17)};
            //printf("adding border to world: %d, %d, %d, %d, %d\n", d[0], d[1], d[2], d[3], d[4]);
            world::buildBorder(d[0], d[1], d[2], d[3], d[4]);
          }
          break;
        case DELETE_BORDER:
          //printf("deleting border %d\n", warp::readIntFromNetworkBuffer(buff->buffer, 1));
          world::deleteBorder(warp::readIntFromNetworkBuffer(buff->buffer, 1));
          break;
        case NEW_TUNNEL:

          break;
        case NEW_TUNNEL_NODE:

          break;
        case NODE_RESOURCE_HARVEST:
          {
            NodeResource *nr = dynamic_cast<NodeResource*>(world::getNodeById(warp::readIntFromNetworkBuffer(buff->buffer, 1)));
            if(nr != NULL){
              nr->getResources(nr->getResourceType(), buff->buffer[5]);
            }
            else{
              printf("CLIENT ERROR: peer harvestet non existant node.\n");
            }
          }
          break;
        case NODE_RESOURCE_REGROWTH:
          {
            NodeResource *nr = dynamic_cast<NodeResource*>(world::getNodeById(warp::readIntFromNetworkBuffer(buff->buffer, 1)));
            if(nr != NULL){
//              printf("storing %d resource in node.%d\n", buff->buffer[5], buff->dataLength);
              nr->storeResource(nr->getResourceType(), buff->buffer[5]);
            }
            else{
              printf("CLIENT ERROR: server let non existent node regrow.\n");
            }
          }
          break;
        case NEW_NODE_HIVE:
          {
            NodeHive *nh = createNodeHiveFromPacket(buff->buffer + 1);
            world::addNode(nh);
            world::registerHive(nh);
          }
          break;
        case ASSIGN_HIVE:
          {
            int hiveId = warp::readIntFromNetworkBuffer(buff->buffer, 1);
            NodeHive *nh = dynamic_cast<NodeHive*>(world::getNodeById(hiveId));
            world::setMainHive(nh);

          }
          break;
        case NEW_ANT: 
          world::spawnNetAnt(warp::readIntFromNetworkBuffer(buff->buffer, 1), warp::readIntFromNetworkBuffer(buff->buffer, 5), warp::readIntFromNetworkBuffer(buff->buffer, 9), warp::readIntFromNetworkBuffer(buff->buffer, 13));
          break;
        case ANT_DEATH:
          world::killNetAnt(warp::readIntFromNetworkBuffer(buff->buffer, 1), warp::readIntFromNetworkBuffer(buff->buffer, 5));

          break;
        case ANT_MOTION:
          // player, id, x, y, dx, dy 
          world::updateNetAnt(warp::readIntFromNetworkBuffer(buff->buffer, 1), warp::readIntFromNetworkBuffer(buff->buffer, 5), warp::readIntFromNetworkBuffer(buff->buffer, 9), warp::readIntFromNetworkBuffer(buff->buffer, 13), warp::readFloatFromNetworkBuffer(buff->buffer, 17), warp::readFloatFromNetworkBuffer(buff->buffer, 21));
          break;
        case NODE_TERRITORY_CONQUERED:
          {
            NodeTerritory *n = dynamic_cast<NodeTerritory *>(world::getNodeById(warp::readIntFromNetworkBuffer(buff->buffer, 1)));
            if(n->getOwner() == world::getPlayerId()){
              world::setNumTerritories(world::getNumTerritories() - 1);
            }
            n->setOwner(warp::readIntFromNetworkBuffer(buff->buffer, 5));
            if(n->getOwner() == world::getPlayerId()){
              world::setNumTerritories(world::getNumTerritories() + 1);
            }

          }
          break;
        case NODE_HIVE_CONQUERED:
          {
            NodeHive *n = dynamic_cast<NodeHive *>(world::getNodeById(warp::readIntFromNetworkBuffer(buff->buffer, 1)));
            n->setOwner(warp::readIntFromNetworkBuffer(buff->buffer, 5));  
          }
          break;
      }

      connectionBuffer.packetLength = 0;
      connectionBuffer.bytesReceived = 0;

    }


    void processPackets(){
      warp::client::update();
    }

    bool isConnected(){
      return warp::client::isConnected();
    }

  }
}

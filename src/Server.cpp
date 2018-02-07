// Copyright 2016 Florian Kramer
#include "Server.h"
#include <string>
#include "warp/os.h"
#include <cstring>
#include <vector>

#ifdef unix
#include <sys/socket.h>
#include <netinet/in.h>
#include <ifaddrs.h>
#include <unistd.h>
#endif

#include <thread>
#include <cstdint>
#include <utility>
#include <map>
#include "Packets.h"
#include <fcntl.h>
#include <errno.h>
#include "NodeResource.h"
#include "NodeTerritory.h"

#include "WorldServer.h"
#include "Node.h"

#include "warp/TimeKeeper.h"
#include "NetAnt.h"

#include "NodeHive.h"
#include "Border.h"

#include "warp/Network.h"


namespace ant{
  namespace server{

    //timing variables
    long current, last; 
    warp::TimeKeeper timeKeeper;
    // unsigned char playerIdCounter = 1;


    void sendWorldToClient(int cid);
    void sendPacket(int clientId, PACKET_TYPE type, unsigned char *data, unsigned int length);

    int socketId = -1;
    sockaddr_in addr;
    // std::vector<std::pair<int, int>> clients; //pairs of cid and socket
    // std::map<int, int> clientMap;
    std::vector<int> clients;
    bool run = true;
    std::thread *listenerThread, *mainThread;
    std::map<int, warp::ConnectionBuffer> packetBuffer;    

    void onClientConnected(unsigned int i) {
      clients.push_back(i);
    }


    void handlePacket(warp::ConnectionBuffer* buff, int clientId){ 

      PACKET_TYPE t = static_cast<PACKET_TYPE>(buff->buffer[0]);
      // printf("server got packet of type %d\n", t);
      switch(t){
        case USERNAME:
          {
            printf("received username from client: %s\n", buff->buffer);

            unsigned char data[1] = {static_cast<unsigned char>(clientId)};

            for(int i : clients){
              if(i != clientId){
                sendPacket(i, PLAYER_JOINED, data, 1);
                unsigned char d[1] = {static_cast<unsigned char>(clientId)};
                sendPacket(clientId, PLAYER_JOINED, d, 1);
              }
            }

            unsigned char d[1] = {static_cast<unsigned char>(clientId)};
            sendPacket(clientId, ID_ASSIGNMENT, d, 1); 
            sendWorldToClient(clientId);
            unsigned char hData[4];
            NodeHive *nh = world::getHiveForPlayer(clientId);
            if(nh != NULL){
              warp::writeIntToNetworkBuffer(nh->getId(), hData, 0);
              sendPacket(clientId, ASSIGN_HIVE, hData, 4);
            }
          }
          break;
        case ID_ASSIGNMENT:

          break;
        case PLAYER_JOINED:

          break;
        case PLAYER_LEFT:
          {
            unsigned char data[buff->dataLength];
            memcpy(data, buff->buffer + 1, buff->dataLength);
            broadcastPacketExcluding(clientId, PLAYER_LEFT, data, buff->dataLength);
            // removeClient(clientId); TODO: add abiltiy to warp to remove clients
            world::removePlayer(clientId);
          }
          break;
        case NEW_NODE_RESSOURCE:

          break;
        case NEW_BORDER:

          break;
        case NEW_TUNNEL:
          { 
            world::digTunnel(warp::readIntFromNetworkBuffer(buff->buffer, 1), warp::readIntFromNetworkBuffer(buff->buffer, 5), warp::readIntFromNetworkBuffer(buff->buffer, 9), warp::readIntFromNetworkBuffer(buff->buffer, 13), buff->buffer[17]);
          }

          break;
        case NEW_TUNNEL_NODE:

          break;
        case NEW_NODE_HIVE:

          break;
        case ASSIGN_HIVE:

          break;
        case NODE_RESOURCE_HARVEST:
          {
            NodeResource *nr = dynamic_cast<NodeResource*>(world::getNodeById(warp::readIntFromNetworkBuffer(buff->buffer, 1)));
            nr->getResources(nr->getResourceType(), buff->buffer[5]);
            broadcastPacketExcluding(clientId, NODE_RESOURCE_HARVEST, buff->buffer + 1, buff->packetLength - 1);
          }
          break;
        case NODE_RESOURCE_REGROWTH:

          break;
        case NEW_ANT: 
          world::spawnNetAnt(warp::readIntFromNetworkBuffer(buff->buffer, 1), warp::readIntFromNetworkBuffer(buff->buffer, 5), warp::readIntFromNetworkBuffer(buff->buffer, 9), warp::readIntFromNetworkBuffer(buff->buffer, 13), buff->buffer[17]);
          broadcastPacketExcluding(clientId, NEW_ANT, buff->buffer + 1, buff->packetLength - 1);
          break;

        case ANT_DEATH:
          world::killNetAnt(warp::readIntFromNetworkBuffer(buff->buffer, 1), warp::readIntFromNetworkBuffer(buff->buffer, 5));
          broadcastPacketExcluding(clientId, ANT_DEATH, buff->buffer + 1, buff->packetLength - 1);

          break;
        case ANT_MOTION:
          // player, id, x, y, dx, dy, currentNodeId
          world::updateNetAnt(
              warp::readIntFromNetworkBuffer(buff->buffer, 1),
              warp::readIntFromNetworkBuffer(buff->buffer, 5),
              warp::readIntFromNetworkBuffer(buff->buffer, 9),
              warp::readIntFromNetworkBuffer(buff->buffer, 13),
              warp::readFloatFromNetworkBuffer(buff->buffer, 17),
              warp::readFloatFromNetworkBuffer(buff->buffer, 21),
              warp::readIntFromNetworkBuffer(buff->buffer, 25));
          broadcastPacketExcluding(clientId, ANT_MOTION, buff->buffer + 1, buff->packetLength - 1);

          break;
        case SET_NODE_COMBAT_STATE:
          {
            Node *n = world::getNodeById(warp::readIntFromNetworkBuffer(buff->buffer, 1));
            n->setCombatPoint(buff->buffer[4] == 1);
          }
          break;
        case NODE_TERRITORY_CONQUERING:
          {
            NodeTerritory *n = dynamic_cast<NodeTerritory*>(world::getNodeById(warp::readIntFromNetworkBuffer(buff->buffer, 1)));
            n->updateConqueringProgressBy(warp::readIntFromNetworkBuffer(buff->buffer, 5), buff->buffer[9]);   
          }
          break;
        case NODE_HIVE_CONQUERING:
          {
            NodeHive *n = dynamic_cast<NodeHive*>(world::getNodeById(warp::readIntFromNetworkBuffer(buff->buffer, 1)));
            n->updateConqueringProgressBy(warp::readIntFromNetworkBuffer(buff->buffer, 5), buff->buffer[9]);   
          }
          break;
        default:
          // printf("SERVER ERROR: received unknown packet type\n");
          break;
      }

      buff->packetLength = 0;
      buff->bytesReceived = 0;
    }

    void mainLoop(){
      timeKeeper.setTargetFPS(30.0);
      timeKeeper.init();

      while(run){
        timeKeeper.beginMeasurement();

        warp::server::readServerPackets(); 
        world::updateWorld(timeKeeper.getUpdateFactor());

        timeKeeper.sleep();
      }
    }

    void start(){
      warp::server::setClientConnectedCallback(onClientConnected);
      warp::server::setServerPacketCallback(handlePacket);
      warp::server::startServer(25542);
      mainThread = new std::thread(mainLoop);
      // TODO move to a more sensible location
      world::generateWorld(2);
    }

    void stop(){
      warp::server::stopServer();
      run = false;
    }

    void sendPacket(int clientId, PACKET_TYPE type, unsigned char *data, unsigned int length){  
      unsigned char *c = new unsigned char[length + 1];
      c[0] = type;
      std::memcpy(c + 1, data, length); 
      warp::server::sendPacket(clientId, c, length + 1);
    }

    void broadcastPacket(PACKET_TYPE type, unsigned char *data, unsigned int length){
      unsigned char *c = new unsigned char[length + 1];
      c[0] = type;
      std::memcpy(c + 1, data, length);
      for(int i : clients){
        warp::server::sendPacket(i, c, length);
      }
    }

    void broadcastPacketExcluding(int clientId, PACKET_TYPE type, unsigned char *data, unsigned int length){
      unsigned char *c = new unsigned char[length + 1];
      c[0] = type;
      std::memcpy(c + 1, data, length);
      for(int i : clients){
        if(i != clientId){
          warp::server::sendPacket(i, c, length);
        }
      }
    }


    bool isRunning(){
      return run == true && socketId != -1;
    }


    void sendWorldToClient(int cid){
      std::vector<Border*> *borders = world::getBorders();  
      std::vector<Node*> *importantNodes = world::getImportantNodes();
      std::map<int, std::map<int, NetAnt*>> *antMap = world::getAntMap();

      //std::vector<NodeHive*>* hives = world::getHives();
      unsigned char data[20]; 
      for(Border *b : (*borders)){
        warp::writeIntToNetworkBuffer(b->getId(), data, 0);
        warp::writeIntToNetworkBuffer(b->getX1(), data, 4);  
        warp::writeIntToNetworkBuffer(b->getY1(), data, 8);  
        warp::writeIntToNetworkBuffer(b->getX2(), data, 12); 
        warp::writeIntToNetworkBuffer(b->getY2(), data, 16); 


        //        printf("sending out border: %d, %d, %d, %d\n", b->getX1(), b->getY1(), b->getX2(), b->getY2());

        sendPacket(cid, NEW_BORDER, data, 20);  
      } 

      for(Node *n  : *importantNodes){
        if(n->getCreationPacketSize() > 0){
          unsigned char *data = new unsigned char[n->getCreationPacketSize()];
          n->writeCreationPacket(data);
          sendPacket(cid, n->getCreationPacketType(), data, n->getCreationPacketSize());  

          delete[] data;  
        }
        else{
          printf("Important node has no implementation for network sync!\n");
        }
      }

      for(auto it = antMap->begin(); it != antMap->end(); ++it){
        for(auto it2 = it->second.begin(); it2 != it->second.end(); ++it2){
          unsigned char data[16];
          warp::writeIntToNetworkBuffer(it->first, data, 0);
          warp::writeIntToNetworkBuffer(it2->second->getId(), data, 4);

          warp::writeIntToNetworkBuffer(it2->second->getX(), data, 8);
          warp::writeIntToNetworkBuffer(it2->second->getY(), data, 12);

          sendPacket(cid, NEW_ANT, data, 16);
        }
      }

    }
  }
}



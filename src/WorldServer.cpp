#include "WorldServer.h"

#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <array>
#include <cstring>

#include "warp/CollisionHandler.h"

#include "NodeTerritory.h"
#include "Node.h"
#include "NodeHive.h"
#include "Border.h"
#include "Tunnel.h"
#include "Server.h"
#include "NodeHive.h"
#include "NodeResource.h"
#include "NetAnt.h"
#include "Circle.h"
#include "MetaBall.h"
#include "util/PerlinNoise.h"
//#include <crand>

#define PI 3.141592653589793

namespace ant{

  namespace server{
    namespace world{

      enum class BlockType {
        EMPTY, DIRT, REINFORCED, WATER, LAVA, ABYSS
      };

      void placeTunnel(Tunnel *t);
      std::vector<Border *> clearArea(Border *pBorders, float borderNormals[4][2], WalledArea *ot, std::vector<Border*>* newBorders = NULL,  std::vector<int>* deletedBorders = NULL);
      void clearTunnel(Tunnel *t, std::vector<Border*>* newBorders = NULL, std::vector<int>* deletedBorders = NULL);
      void clearNode(TunnelNode *t, std::vector<Border*>* newBorders = NULL, std::vector<int>* deletedBorders = NULL);


      int centerX = 0, centerY = 0;

      std::map<unsigned int, Node*> nodes;
      std::vector<Node *> importantNodes;
      std::vector<NetAnt *> ants;
      std::map<int, std::map<int, NetAnt *>> antMap;

      std::vector<Border *> borders;
      std::vector<NodeHive *> hives;

      std::vector<TunnelNode *> tunnelNodes;
      std::vector<Tunnel *> tunnels;

      std::map<int, NodeHive *> playerHives;

      unsigned int blockCountWidth, blockCountHeight;
      unsigned char **blocks;

      void updateWorld(float updateFactor) {
        //        for (NetAnt *a : ants) {
        //          a->update(updateFactor);
        //        }

        for (Node * n : importantNodes) {
          n->update(updateFactor);
        }
      }


      void spawnNetAnt(int player, int id, int x, int y, int strength) {

        NetAnt *a = new NetAnt(x, y, strength, id, player);
        ants.push_back(a);
        antMap[player][id] = a;
      }

      void updateNetAnt(int player, int id, int x, int y, float dx, float dy, int currentNodeId) {
        auto t = antMap[player].find(id);
        if(t != antMap[player].end()) {
          t->second->setBaseX(x);
          t->second->setBaseY(y);
          t->second->setOffsetX(0);
          t->second->setOffsetY(0);
          t->second->setSpeedX(dx);
          t->second->setSpeedY(dy);
          if(currentNodeId >= 0) {
            t->second->setCurrentNode(getNodeById(currentNodeId));
          }
          else{
            t->second->setCurrentNode(NULL);
          }
        }

      }

      NodeHive *getHiveForPlayer(int pId) {
        std::map<int, NodeHive*>::iterator it;
        if((it = playerHives.find(pId)) != playerHives.end()) {
          return it->second;
        }
        else{
          playerHives.emplace(pId, hives[playerHives.size()]);
          return playerHives[pId];
        }

      }

      void killNetAnt(int player, int id) {
        auto t = antMap[player].find(id);
        if(t != antMap[player].end()) {
          t->second->onDeath();
          ants.erase(std::remove(ants.begin(), ants.end(), t->second), ants.end());
          antMap[player].erase(t);
          //t->second->die();
        }
      }

      bool intersectsBorder(int x1, int y1, int x2, int y2) {
        Border temp(x1, y1, x2, y2);
        for (Border *b : borders) {
          if(b->intersectsWith(&temp)) {
            warp::drawLine(b->getX1(), b->getY1(), b->getX2(), b->getY2(), 3);
            return true;
          }
        }
        return false;
      }

      Node *getNodeAt(int x,int y, int tolerance) {
        double dist = tolerance;
        Node *node = NULL;
        for (auto n : nodes) {
          //if(hypot(x - n.second->getX(), y - n.second->getY()) < dist)
          if(std::hypot(x - n.second->getX(), y -  n.second->getY()) < dist) {
            //dist = hypot(x - n.second->getX(), y - n.second->getY());
            dist = std::hypot(x - n.second->getX(), y - n.second->getY());
            node = n.second;
          }
        }
        return node;
      }


      Node *createNodeAt(int x, int y) {
        Node *n = new Node(x, y, true);
        nodes[n->getId()] = n;
        return n;
      }

      void deleteNodeAt(int x, int y) {
        Node *node = getNodeAt(x, y, 1000);
        if(node != NULL) {
          node->isolate();
          if(node->isDeletable()) {
            nodes.erase(nodes.find(node->getId()));
            if(find(importantNodes.begin(), importantNodes.end(), node) != importantNodes.end()) {
              importantNodes.erase(std::remove(importantNodes.begin(), importantNodes.end(), node), importantNodes.end());
            }
            delete node;
          }
        }
      }

      Node *getNodeById(int id) {
        return nodes[id];
      }

      int getCenterX() {
        return centerX;
      }

      int getCenterY() {
        return centerY;
      }


      void removePlayer(int id) {
        for (auto p : antMap[id]) {
          unsigned char data[8]; //TODO find elegant way to have ant itself send death signal from server
          writeIntToBuffer(id, data, 0);
          writeIntToBuffer(p.second->getId(), data, 4);
          broadcastPacketExcluding(id, ANT_DEATH, data, 8);
          delete p.second;
        }
        antMap[id].clear();
        antMap.erase(antMap.find(id));

      }



      void digTunnel(int x1, int y1, int x2, int y2, int width) {
        Tunnel *t = new Tunnel();

        float length = std::hypot(x1 - x2, y1 - y2);
        float vn[2] = {(x2 - x1) / length, (y2 - y1) / length};
        //x1 -= vn[0] * 2 * width; // ensure hole in the wall is full sized, by ensuring that the tunnel pokes through
        //y1 -= vn[1] * 2 * width; // the walls at both ends by at least width. Only works if it is the smallest type
        //x2 += vn[0] * 2 * width; // of tunnel on the map
        //y2 += vn[1] * 2 * width;

        t->x1 = x1;
        t->y1 = y1;
        t->x2 = x2;
        t->y2 = y2;
        t->width = width;

        tunnels.push_back(t);

        std::vector<int> deletedBorders;
        std::vector<Border *> newBorders;

        placeTunnel(t);
        newBorders.insert(newBorders.end(), t->borders.begin(), t->borders.end());

        //clearTunnel(t, &newBorders, &deletedBorders);

        for (Tunnel *ot : tunnels) {
          //clearTunnel(ot, &newBorders, &deletedBorders);
        }
        for (TunnelNode *ot : tunnelNodes) {
          //clearNode(ot, &newBorders, &deletedBorders);
        }

        std::vector<int> sentIds;

        unsigned char ddata[4];
        for (int i : deletedBorders) {
          if(std::find(sentIds.begin(), sentIds.end(), i) == sentIds.end()) {
            //printf("sending delete for border %d\n", i);
            sentIds.push_back(i);
            writeIntToBuffer(i, ddata, 0);
            broadcastPacket(DELETE_BORDER, ddata, 4);
          }
        }

        sentIds.clear();
        unsigned char ndata[20];
        for (Border *b : newBorders) {
          if(std::find(sentIds.begin(), sentIds.end(), b->getId()) == sentIds.end()) {
            //printf("sending create for border %d\n", b->getId());
            sentIds.push_back(b->getId());
            writeIntToBuffer(b->getId(), ndata, 0);
            writeIntToBuffer(b->getX1(), ndata, 4);
            writeIntToBuffer(b->getY1(), ndata, 8);
            writeIntToBuffer(b->getX2(), ndata, 12);
            writeIntToBuffer(b->getY2(), ndata, 16);

            broadcastPacket(NEW_BORDER, ndata, 20);
          }
        }
      }


      //WORLD CREATION
      //========================================================================


      std::vector<Border*> mergeAndTesselateCircles(std::vector<Circle> &circles) {
        std::vector<Border*> newBorders;

        MetaBall m;
        for (const Circle &c : circles) {
          m.addMetaBall(c.getX(), c.getY(), c.getRadius() * 4);
        }
        std::vector<std::array<float, 4>> lines = m.tesselate();
        for (auto &a : lines) {
          Border *b = new Border(a[0], a[1], a[2], a[3]);
          newBorders.push_back(b);
        }
        return newBorders;
      }



      void spawnPlayerAt(int x, int y) {

        int hiveNodeSize = 700;

        NodeHive *h = new NodeHive(x, y, -1, true);
        nodes[h->getId()] = h;
        importantNodes.push_back(h);
        hives.push_back(h);


        TunnelNode *n = new TunnelNode();
        n->x = x;
        n->y = y;
        n->size = hiveNodeSize;
        tunnelNodes.push_back(n);

        std::vector<std::pair<int, int>> resPos;

        float a = 0;
        for (int i = 0; i < 5; i++) {
          a += (std::rand() % 6283) / 1000.0;
          float radius = (std::rand() % 1000) / 1000.0 * (hiveNodeSize - 400) + 330;

          int nx = x + static_cast<int>(std::cos(a) * radius);
          int ny = y + static_cast<int>(std::sin(a) * radius);
          bool fits = true;
          for (std::pair<int, int> p : resPos) {
            if(hypot(p.first - nx, p.second - ny) < 70) {
              fits = false;
              break;
            }
          }
          if(fits) {
            NodeResource *r = new NodeResource(nx, ny , std::rand() % NUM_RESOURCES, true);
            resPos.push_back(std::pair<int, int>(r->getX(), r->getY()));
            importantNodes.push_back(r);
            nodes[r->getId()] = r;
          }
        }


        //Hive borders

        Border *b1 = new Border(x - 200, y + 200, x - 25, y + 200); //top
        Border *b2 = new Border(x + 25, y + 200, x + 200, y + 200);

        Border *b3 = new Border(x - 200, y + 200, x - 200, y + 25); //left
        Border *b4 = new Border(x - 200, y - 25, x - 200, y - 200);

        Border *b5 = new Border(x - 200, y - 200, x - 25, y - 200); //bottom
        Border *b6 = new Border(x + 25, y - 200, x + 200, y - 200);

        Border *b7 = new Border(x + 200, y + 200, x + 200, y + 25); //right
        Border *b8 = new Border(x + 200, y - 25, x + 200, y - 200);


        Border *b9 = new Border(x - 210, y + 210, x - 25, y + 210); //top outer
        Border *b10 = new Border(x + 25, y + 210, x + 210, y + 210);

        Border *b11 = new Border(x - 210, y + 210, x - 210, y + 25); //left outer
        Border *b12 = new Border(x - 210, y - 25, x - 210, y - 210);

        Border *b13 = new Border(x - 210, y - 210, x - 25, y - 210); //bottom outer
        Border *b14 = new Border(x + 25, y - 210, x + 210, y - 210);

        Border *b15 = new Border(x + 210, y + 210, x + 210, y + 25); //right outer
        Border *b16 = new Border(x + 210, y - 25, x + 210, y - 210);

        borders.push_back(b1);
        borders.push_back(b2);
        borders.push_back(b3);
        borders.push_back(b4);
        borders.push_back(b5);
        borders.push_back(b6);
        borders.push_back(b7);
        borders.push_back(b8);

        borders.push_back(b9);
        borders.push_back(b10);
        borders.push_back(b11);
        borders.push_back(b12);
        borders.push_back(b13);
        borders.push_back(b14);
        borders.push_back(b15);
        borders.push_back(b16);


        //start area borders
        Border *sl = new Border(x - hiveNodeSize, y - hiveNodeSize, x - hiveNodeSize, y + hiveNodeSize);
        Border *st = new Border(x - hiveNodeSize, y + hiveNodeSize, x + hiveNodeSize, y + hiveNodeSize);
        Border *sr = new Border(x + hiveNodeSize, y + hiveNodeSize, x + hiveNodeSize, y - hiveNodeSize);
        Border *sb = new Border(x - hiveNodeSize, y - hiveNodeSize, x + hiveNodeSize, y - hiveNodeSize);

        n->borders.push_back(st);
        n->borders.push_back(sl);
        n->borders.push_back(sr);
        n->borders.push_back(sb);

        borders.push_back(sl);
        borders.push_back(st);
        borders.push_back(sr);
        borders.push_back(sb);
      }


      TunnelNode *spawnResourceSpot(int x, int y, unsigned int *seed) { 
        std::vector<Circle> circles;
        int numRes = rand() % 3 + 1;
        int numStructuralCenters;
        int spotSize = 420;
        switch(rand() % 3) {
          case 0: // small
            spotSize = 270 + rand() % 40;
            numRes = 1 + (rand() % 2);
            numStructuralCenters = 4;
            break;
          case 1: // medium
            spotSize = 350 + rand() % 90;
            numRes = 2 + (rand() % 3);
            numStructuralCenters = 6;
            break;
          case 2: // large
            spotSize = 500 + rand() % 150;
            numRes = 3 + (rand() % 2);
            numStructuralCenters = 8;
            break;
        }

        float angle = 0;
        // generate actual resource nodes
        for (int i = 0; i < numRes; i++) {
          float rad = (rand() % 1000) / 1000.0 * 0.4 * spotSize + 0.25 * spotSize;
          NodeResource *n = new NodeResource(cos(angle) * rad + x, sin(angle) * rad + y, rand() % NUM_RESOURCES, true);
          nodes[n->getId()] = n;
          importantNodes.push_back(n);
          angle += 2 * PI / numRes;
          circles.push_back(Circle(n->getX(), n->getY(), rand_r(seed) % 150 + 100));
        }

        for (int i = 0; i < numStructuralCenters; i++) {
          float rad = (rand() % 1000) / 1000.0 * 0.4 * spotSize + 0.25 * spotSize;
          angle += 2 * PI / numRes * ((rand() % 100) / 100.0 + 0.5); 
          circles.push_back(Circle(cos(angle) * rad + x, sin(angle) * rad + y, rand_r(seed) % 150 + 100));
        }

        std::vector<Border *> newBorders = mergeAndTesselateCircles(circles);
        borders.insert(borders.end(), newBorders.begin(), newBorders.end());


        // legacy tunnel node stuff
        TunnelNode *n = new TunnelNode();
        n->size = spotSize;
        n->x = x;
        n->y = y;

        // add conquerable node to the center
        NodeTerritory *nt = new NodeTerritory(x, y, true);
        nodes[nt->getId()] = nt;
        importantNodes.push_back(nt);
        return n;
      }

      void placeTunnel(Tunnel *t) {
        int dx = t->x2 - t->x1;
        int dy = t->y2 - t->y1;
        float l = hypot(dx, dy);
        float px = dy / l;
        float py = -dx / l;
        //int tunnelWidth = 30 + rand() % ((t->nodes[0]->size > t->nodes[1]->size ? t->nodes[1]->size : t->nodes[0]->size) - 35);
        if(t->width == -1) {
          int tunnelWidth = 50 + rand() % 100;
          if(tunnelWidth > t->nodes[0]->size) {
            tunnelWidth = t->nodes[0]->size;
          }
          if(tunnelWidth > t->nodes[1]->size) {
            tunnelWidth = t->nodes[1]->size;
          }

          t->width = tunnelWidth;
        }
        //        printf("tunnel dimensions: %d, %d\n", t->x1, t->x2);

        Border *tbs[2];

        tbs[0] = new Border(t->x1 + static_cast<int>(px * t->width), t->y1 + static_cast<int>(py * t->width), t->x2 + static_cast<int>(px * t->width), t->y2 + static_cast<int>(py * t->width));
        tbs[1] = new Border(t->x1 - static_cast<int>(px * t->width), t->y1 - static_cast<int>(py * t->width), t->x2 - static_cast<int>(px * t->width), t->y2 - static_cast<int>(py * t->width));
        borders.push_back(tbs[0]);
        borders.push_back(tbs[1]);
        t->borders.push_back(tbs[0]);
        t->borders.push_back(tbs[1]);
      }


      //clears rectangle described by pBorders of borders and returns newly created borders
      std::vector<Border *> clearArea(Border *pBorders, float borderNormals[4][2], WalledArea *ot, std::vector<Border*>* newBorders,  std::vector<int>* deletedBorders) {

        std::vector<Border *> createdBorders;
        std::vector<Border *> bordersToDelete;
        for (Border *b : ot->borders) {
          std::vector<std::pair<int*, int>> intersections; //vector of intersection point, pBorder index pairs
          for (unsigned int it = 0; it < 4; it++) {
            Border *tb = &pBorders[it];
            if(b->intersectsWith(tb)) {
              intersections.push_back(std::pair<int*, int>(b->getIntersectionWith(tb), it));
            }
          }

          if(intersections.size() == 1) {
            //            printf("class 1 intersection\n");
            std::pair<int*, int> is = intersections[0];
            if((b->getX1() - is.first[0]) * borderNormals[is.second][0] + (b->getY1() - is.first[1]) * borderNormals[is.second][1] > 0) {
              b->setX1(is.first[0]);
              b->setY1(is.first[1]);
            }
            else{
              b->setX2(is.first[0]);
              b->setY2(is.first[1]);
            }
            if(deletedBorders != NULL) {
              deletedBorders->push_back(b->getId());
            }
            if(newBorders != NULL) {
              newBorders->push_back(b);
            }

          }
          else if(intersections.size() == 2) {
            //            printf("class 2 intersection\n");
            std::pair<int*, int> is = intersections[0];
            if((b->getX1() - is.first[0]) * borderNormals[is.second][0] + (b->getY1() - is.first[1]) * borderNormals[is.second][1] > 0) {
              Border *nb = new Border(b->getX1(), b->getY1(), intersections[1].first[0], intersections[1].first[1]);
              createdBorders.push_back(nb);
              b->setX1(is.first[0]);
              b->setY1(is.first[1]);
            }
            else{
              Border *nb = new Border(b->getX2(), b->getY2(), intersections[1].first[0], intersections[1].first[1]);
              createdBorders.push_back(nb);
              b->setX2(is.first[0]);
              b->setY2(is.first[1]);
            }
            if(deletedBorders != NULL) {
              deletedBorders->push_back(b->getId());
            }
            if(newBorders != NULL) {
              newBorders->push_back(b);
            }

          }
          else if(intersections.size() == 0) {
            //            printf("class 0 intersection\n");
            bool inside = true;
            for (unsigned int it = 0; it < 4; it++) {
              Border *tb = &pBorders[it];
              if((b->getX1() - tb->getX1()) * borderNormals[it][0] + (b->getY1() - tb->getY1()) * borderNormals[it][1] < 0) {
                inside = false;
                break;
              }
              if((b->getX2() - tb->getX1()) * borderNormals[it][0] + (b->getY2() - tb->getY1()) * borderNormals[it][1] < 0) {
                inside = false;
                break;
              }
            }
            if(inside) {
              //              printf("border encased, deleting\n");
              bordersToDelete.push_back(b);
            }

          }
          else{
            printf("ERROR during tunnel clearing: line intersected more than 2 walls of parallelogram\n");
          }

          for (auto a : intersections) {
            delete[] a.first;
          }
        }
        for (Border *b : bordersToDelete) {
          if(deletedBorders != NULL) {
            deletedBorders->push_back(b->getId());
          }
          if(newBorders != NULL) {
            newBorders->erase(std::remove(newBorders->begin(), newBorders->end(), b), newBorders->end());
          }
          borders.erase(std::remove(borders.begin(), borders.end(), b), borders.end());
          ot->borders.erase(std::remove(ot->borders.begin(), ot->borders.end(), b), ot->borders.end());
          delete b; //TODO check for potential references i overlooked or problems wit hdeleting the llop object
        }

        return createdBorders;
      }



      void clearTunnel(Tunnel *t, std::vector<Border*>* newBorders, std::vector<int>* deletedBorders) {
        int dx = t->x2 - t->x1;
        int dy = t->y2 - t->y1;
        float length = hypot(dx, dy);
        float crossVectorN[2] = {dy / length, -dx / length};
        float crossVector[2] = {crossVectorN[0] * t->width, crossVectorN[1] * t->width};
        Border pBorders[4] = {
          Border(t->x1 + crossVector[0], t->y1 + crossVector[1], t->x2 + crossVector[0], t->y2 + crossVector[1]),
          Border(t->x1 - crossVector[0], t->y1 - crossVector[1], t->x2 - crossVector[0], t->y2 - crossVector[1]),
          Border(t->x1 + crossVector[0], t->y1 + crossVector[1], t->x1 - crossVector[0], t->y1 - crossVector[1]),
          Border(t->x2 + crossVector[0], t->y2 + crossVector[1], t->x2 - crossVector[0], t->y2 - crossVector[1])
        };

        float borderNormals[4][2] = { //inwards facing normals of pBorders
          {-crossVectorN[0], -crossVectorN[1]},
          {crossVectorN[0], crossVectorN[1]},
          {dx / length, dy / length},
          {-dx / length, -dy / length}
        };


        for (Tunnel *ot : tunnels) {
          if(ot != t) {
            std::vector<Border *> createdBorders = clearArea(pBorders, borderNormals, ot, newBorders, deletedBorders);
            for (Border *nb : createdBorders) {
              ot->borders.push_back(nb);
              borders.push_back(nb);
              if(newBorders != NULL) {
                newBorders->push_back(nb);
              }
            }
          }
        }
        for (TunnelNode *ot : tunnelNodes) {
          std::vector<Border *> createdBorders = clearArea(pBorders, borderNormals, ot, newBorders, deletedBorders);
          for (Border *nb : createdBorders) {
            ot->borders.push_back(nb);
            borders.push_back(nb);
            if(newBorders != NULL) {
              newBorders->push_back(nb);
            }
          }
        }
      }

      void clearNode(TunnelNode *t, std::vector<Border*>* newBorders, std::vector<int>* deletedBorders) {
        Border pBorders[4] = {
          Border(t->x - t->size, t->y - t->size, t->x - t->size, t->y + t->size),
          Border(t->x - t->size, t->y + t->size, t->x + t->size, t->y + t->size),
          Border(t->x + t->size, t->y - t->size, t->x + t->size, t->y + t->size),
          Border(t->x - t->size, t->y - t->size, t->x + t->size, t->y - t->size)

        };

        float borderNormals[4][2] = { //inwards facing normals of pBorders
          {1, 0},
          {0, -1},
          {-1, 0},
          {0, 1}
        };

        for (Tunnel *ot : tunnels) {
          std::vector<Border *> createdBorders = clearArea(pBorders, borderNormals, ot, newBorders, deletedBorders);
          for (Border *nb : createdBorders) {
            ot->borders.push_back(nb);
            borders.push_back(nb);
            if(newBorders != NULL) {
              newBorders->push_back(nb);
            }
          }

        }

        for (TunnelNode *ot : tunnelNodes) {
          if(ot != t) {

            std::vector<Border *> createdBorders = clearArea(pBorders, borderNormals, ot, newBorders, deletedBorders);
            for (Border *nb : createdBorders) {
              ot->borders.push_back(nb);
              borders.push_back(nb);
              if(newBorders != NULL) {
                newBorders->push_back(nb);
              }
            }
          }
        }
      }

      void generateWorld(int numPlayers) {

        int mapSize = numPlayers * 2480;
        int numNodesPerPlayer = 5;

        blockCountWidth = mapSize / 40;
        blockCountHeight = mapSize / 40;
        blocks = new unsigned char *[blockCountWidth];
        for (unsigned int i = 0; i < blockCountWidth; i++) {
          blocks[i] = new unsigned char[blockCountHeight];
          std::memset(blocks[i], 0, blockCountHeight * sizeof(blocks[i][0]));
        }

        Border *l = new Border(-mapSize, -mapSize, -mapSize, mapSize);
        Border *b = new Border(-mapSize, -mapSize, mapSize, -mapSize);
        Border *t = new Border(-mapSize, mapSize, mapSize, mapSize);
        Border *r = new Border(mapSize, -mapSize, mapSize, mapSize);
        Border *lo = new Border(-(mapSize + 5), -(mapSize + 5), -(mapSize + 5), (mapSize + 5));
        Border *bo = new Border(-(mapSize + 5), -(mapSize + 5), (mapSize + 5), -(mapSize + 5));
        Border *to = new Border(-(mapSize + 5), (mapSize + 5), (mapSize + 5), (mapSize + 5));
        Border *ro = new Border((mapSize + 5), -(mapSize + 5), (mapSize + 5), (mapSize + 5));
        borders.push_back(l);
        borders.push_back(b);
        borders.push_back(t);
        borders.push_back(r);
        borders.push_back(lo);
        borders.push_back(bo);
        borders.push_back(to);
        borders.push_back(ro);


        int hiveRadius = 0.7 * mapSize;
        float hiveAngle = 1.5707963267948966;
        float aSlizeSize = 6.283185307179586 / numPlayers;

        // TODO debug code remove
        /*Circle c1(50, 10, 200);
        Circle c2(-100, 300, 200); 
        std::vector<Circle> circles;
        circles.push_back(c1);
        circles.push_back(c2);
        std::vector<Border*> bs = mergeAndTesselateCircles(circles);
        borders.insert(borders.begin(), bs.begin(), bs.end());*/



        unsigned int seed = std::time(0); 
        // seed = 1467113370;
        std::srand(seed);
        printf("World seed: %d\n", seed);

        // generate rivers
        if (rand() % 3 > 0 || true){
          float angle = (rand() + 0.0) / RAND_MAX * 2 * PI;
          float angleDelta = (rand() + 0.0) / RAND_MAX * 2 - 1;
          angleDelta = 0;
          float angleGoal = angle + PI + angleDelta;
          while (angleGoal > 2 * PI) {
            angleGoal -= 2 * PI;
          }
          while (angleGoal < 0) {
            angleGoal += 2 * PI;
          }

          int baseX, baseY;
          int goalX, goalY;
          if (angle > PI / 4) {
            baseX = (angle - PI / 4) / PI / 2 * mapSize;
            baseY = mapSize;
          } else if (angle > 3 * PI / 4) {
            baseX = -mapSize;
            baseY = (angle - 3 * PI / 4) / PI / 2 * mapSize;
          } else if (angle > 5 * PI / 4) {
            baseX = (angle - 5 * PI / 4) / PI / 2 * mapSize;
            baseY = -mapSize;
          } else if (angle > 7 * PI / 4 || angle < PI / 4) {
            baseX = mapSize;
            baseY = (angle - 7 * PI / 4) / PI / 2 * mapSize;
          } else {
            baseX = 0;
            baseY = 0;
          } 

          if (angleGoal > PI / 4) {
            goalX = (angleGoal - PI / 4) / PI / 2 * mapSize;
            goalY = mapSize;
          } else if (angleGoal > 3 * PI / 4) {
            goalX = -mapSize;
            goalY = (angleGoal - 3 * PI / 4) / PI / 2 * mapSize;
          } else if (angleGoal > 5 * PI / 4) {
            goalX = (angleGoal - 5 * PI / 4) / PI / 2 * mapSize;
            goalY = -mapSize;
          } else if (angleGoal > 7 * PI / 4 || angleGoal < PI / 4) {
            goalX = mapSize;
            goalY = (angleGoal - 7 * PI / 4) / PI / 2 * mapSize;
          } else {
            goalX = 0;
            goalY = 0;
          }

          baseX *= 1.2;
          baseY *= 1.2;

          goalX = -baseX;
          goalY = -baseY;

          int dx = goalX - baseX;
          int dy = goalY - baseY;
          float ox = dy;
          float oy = -dx;
          ox = ox / std::hypot(ox, oy);
          oy = oy / std::hypot(ox, oy);

          PerlinNoise p;

          int numSamples = 100;

          float sx = dx / (numSamples + 0.0);
          float sy = dy / (numSamples + 0.0);

          float cx = baseX, cy = baseY;
          float nx, ny;

          float noisePos1 = 0;
          int noiseAmp1 = 1000;
          float noiseVal1;

          float noisePos2 = 0;
          int noiseAmp2 = 4000;
          float noiseVal2;


          float riverWidth = 700;

          float ofx, ofy;

          float noiseFactor = 1;
          float oldNoiseX = 0, oldNoiseY = 0;
          for (int i = 0; i < numSamples; i++) {
            noiseFactor = i < 9 ? (i + 1) / 10.0 : i > 90 ?  1 - (i - 90) / 10.0 : 1;

            noiseVal1 = p.value(noisePos1, 0) * 2 - 1;
            noiseVal2 = p.value(noisePos2, 1) * 2 - 1;

            ofx = (noiseVal1 * noiseAmp1 + noiseVal2 * noiseAmp2) * noiseFactor * ox;
            ofy = (noiseVal1 * noiseAmp1 + noiseVal2 * noiseAmp2) * noiseFactor * oy;

            nx = cx + sx;
            ny = cy + sy;

            Border *b = new Border(
                cx + oldNoiseX - riverWidth * ox,
                cy + oldNoiseY - riverWidth * oy,
                nx + ofx - riverWidth * ox,
                ny + ofy - riverWidth * oy);
            borders.push_back(b);
            b = new Border(
                cx + oldNoiseX + riverWidth * ox,
                cy + oldNoiseY + riverWidth * oy,
                nx + ofx + riverWidth * ox,
                ny + ofy + riverWidth * oy);
            borders.push_back(b);


            cx = nx;
            cy = ny;
            oldNoiseX = ofx;
            oldNoiseY = ofy;
            noisePos1 += 5.0 / numSamples;
            noisePos2 += 1.0 / numSamples;
          }
            Border *b = new Border(cx, cy, goalX, goalY);
            borders.push_back(b);
        }
        // spawn hives and resource spots
        for (int i = 0; i < numPlayers; i++) {

          std::srand(seed);
          spawnPlayerAt(cos(hiveAngle) * hiveRadius, sin(hiveAngle) * hiveRadius);
          hiveAngle += aSlizeSize;

          float minAngle = 1.5707963267948966 - aSlizeSize / 2 + i * aSlizeSize;
          float angle = fmod((rand() % 100000) / 1000.0 , aSlizeSize);

          int misfits = 0;
          for (int j = 0; j < numNodesPerPlayer && misfits < 100; j++) {
            float absSin = fabs(sin(minAngle + angle));
            float absCos = fabs(cos(minAngle + angle));
            float l = 1;
            if(mapSize * absSin <= mapSize * absCos) {
              l = mapSize / absCos;
            }
            else{
              l = mapSize / absSin;
            }
            l -= 300; // keep nodes away from the border
            float radius = (rand() % static_cast<int>(l));
            bool fits = true;
            int nnx = cos(minAngle + angle) * radius;
            int nny = sin(minAngle + angle) * radius;
            for (TunnelNode *t : tunnelNodes) {
              //          if( hypot(t->x - nnx, t->y - nny)  < 500) {
              //            printf("checking %f < %f\n",hypot(t->x - nnx, t->y - nny), t->size * 1.6);
              //          }
              if(hypot(t->x - nnx, t->y - nny) < hypot(t->size, t->size) + 600) {
                fits = false;
                break;
              }
            }

            if(fits) {
              //          printf("Node ok\n"); 
              TunnelNode *t = spawnResourceSpot(nnx, nny, &seed);
              tunnelNodes.push_back(t);
              misfits = 0;
            }
            else{
              //          printf("Node doesn't fit\n");
              j --;
              misfits ++;
            }
            angle += (fmod(rand(), 100000)) / 1000.0;
            angle = fmod(angle, aSlizeSize);
          }
          if(misfits > 99) {
            // printf("WARNING: Aborted adding resource nodes. Map full.\n");
          }
        }

        return;
        //connect the Tunnel Nodes

        for (TunnelNode *n : tunnelNodes) {
          std::vector<TunnelNode *> connectionCandidates;

          // Filter based upon distance
          for (TunnelNode *n2 : tunnelNodes) {
            if(n2 != n) {
              if(hypot(n2->x - n->x, n2->y - n->y) < 4000) {
                connectionCandidates.push_back(n2);
              }
            }
          }

          // Filter based upon direction
          std::vector<TunnelNode *> toBeRemoved;
          for (TunnelNode *cn : connectionCandidates) {
            float a1 = std::atan2(cn->y - n->y, cn->x - n->x);
            for (TunnelNode *cn2 : connectionCandidates) {
              if(cn != cn2) {
                float a2 = std::atan2(cn2->y - n->y, cn2->x - n->x);

                if(std::fabs(a1 - a2) < 1.5) {// angle at which to connections are to close
                  if(hypot(cn->x - n->x, cn->y - n->y) < hypot(cn2->x - n->x, cn2->y - n->y)) {
                    toBeRemoved.push_back(cn2);
                  }
                  else{
                    toBeRemoved.push_back(cn);
                  }
                }
              }
            }
          }
          for (TunnelNode *cn : toBeRemoved) {
            connectionCandidates.erase(std::remove(connectionCandidates.begin(), connectionCandidates.end(), cn), connectionCandidates.end());
          }


          //create tunnels to connection Candidates
          for (TunnelNode *cn : connectionCandidates) {
            bool tunnelExists = false;
            for (Tunnel *t : cn->tunnels) {
              if(t->nodes[0] == n || t->nodes[1] == n) {
                tunnelExists = true;
                break;
              }
            }
            if(!tunnelExists) {
              Tunnel *t = new Tunnel();
              t->nodes[0] = n;
              t->nodes[1] = cn;
              t->x1 = n->x;
              t->y1 = n->y;
              t->x2 = cn->x;
              t->y2 = cn->y;
              n->tunnels.push_back(t);
              cn->tunnels.push_back(t);
              tunnels.push_back(t);
            }
          }
        }

        //build tunnels
        for (Tunnel *t : tunnels) {
          placeTunnel(t);
        }
        for (Tunnel *t : tunnels) {
          clearTunnel(t);
        }
        for (TunnelNode *t : tunnelNodes) {
          clearNode(t);
        }
      }


      void cleanupWorld() {
        for (auto a : nodes) {
          delete a.second;
        }
        for (NetAnt *a : ants) {
          delete a;
        }
        for (Border *b : borders) {
          delete b;
        }
        for (TunnelNode *n : tunnelNodes) {
          delete n;
        }
        for (Tunnel *t : tunnels) {
          delete t;
        }
        for (unsigned int i = 0; i < blockCountWidth; i++) {
          delete[] blocks[i];
        }
        delete[] blocks;
        blockCountWidth = 0;
        blockCountHeight = 0;
        blocks = NULL;

        nodes.clear();
        importantNodes.clear();
        hives.clear();
        ants.clear();
        borders.clear();
        tunnelNodes.clear();
        tunnels.clear();
      }


      void buildBorder(int x1, int y1, int x2, int y2) {
        Border *b = new Border(x1, y1, x2, y2);
        borders.push_back(b);
      }


      std::vector<Border*>* getBorders() {
        return &borders;
      }

      std::vector<Node*>* getImportantNodes() {
        return &importantNodes;
      }

      std::vector<NodeHive*>* getHives() {
        return &hives;
      }

      void addNode(Node *n) {
        nodes[n->getId()] = n;
        if(n->isInteresting()) {
          importantNodes.push_back(n);
        }
      }


      void newPlayer(int id) {
        antMap.insert(std::pair<int, std::map<int, NetAnt*>>(id, std::map<int, NetAnt*>()));
      }

      std::map<int, std::map<int, NetAnt*>> *getAntMap() {
        return &antMap;
      }
    }
  }
}

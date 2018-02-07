// Copyright 2016 Florian Kramer
//
#include "WorldClient.h"

#include <vector>
#include <map>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <ctime>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "warp/Renderer.h"
#include "warp/Warp.h"
#include "warp/FBOHandler.h"
#include "warp/GraphicsOptions.h"

#include "NodeHive.h"
#include "NodeResource.h"
#include "NetAnt.h"
#include "TaskWander.h"
#include "Tunnel.h"
#include "WorldServer.h"
#include "Options.h"

#define PI 3.141592653589793

namespace ant{
  namespace world{
    warp::Color worldColorBlack(0, 0, 0, 1);

    int playerId = 0;

    int centerX = 0, centerY = 0;

    std::map<unsigned int, Node*> nodes;
    std::vector<Node *> importantNodes;
    std::vector<NetAnt *> ants;
    std::map<int, std::map<int, NetAnt *>> antMap;
    std::vector<NetAnt *> deadAnts;


    //std::vector<Task *> tasks;
    std::vector<Task *> tasks[NUM_TASK_TYPES];
    std::vector<Border *> borders;
    std::vector<NodeHive *> hives;

    std::vector<TunnelNode *> tunnelNodes;
    std::vector<Tunnel *> tunnels;
    NodeHive *mainHive = NULL;

    
    enum class BlockType {
      EMPTY, DIRT, REINFORCED, WATER, LAVA, ABYSS
    };

    warp::Color colorEmpty(1, 1, 1, 1);
    warp::Color colorDirt(0.7, 0.35, 0.1, 1);
    warp::Color colorReinforced(0.5, 0.5, 0.5, 1);
    warp::Color colorWater(0, 0, 1, 1);
    warp::Color colorLava(1, 0, 0, 1);
    warp::Color colorAbyss(0, 0, 0, 1);

    warp::Color worldColors[] = {
      colorEmpty,
      colorDirt,
      colorReinforced,
      colorWater,
      colorLava,
      colorAbyss
    };

    const unsigned int blockSize = 40;
    unsigned int blockCountWidth, blockCountHeight;
    unsigned char **blocks = NULL;

    unsigned int blockDirtRenderable = -1;
    unsigned int blockReinforcedRenderable = -1;
    unsigned int blockWaterRenderable = -1;
    unsigned int blockLavaRenderable = -1;
    unsigned int blockAbyssRenderable = -1;

    int lastAntId = 0;

    int numTerritories = 0;

    int borderRenderable = -1;
    warp::Color antCountColor(0, 0, 0, 1);

    unsigned int fogOfWarFboId = -1;
    bool fogOfWarEnabled = false;

    void renderWorld(){
      warp::setRenderTarget(warp::WORLD);

      // check if fogOfWar fbo exists
      if (fogOfWarFboId == static_cast<unsigned int>(-1)) {
        fogOfWarFboId = warp::fbo::obtainFboId();
        warp::createFboForId(fogOfWarFboId,
            warp::fbo::getDefaultFboWidth(), warp::fbo::getDefaultFboHeight());
      }

      for(auto n : nodes){
        n.second->render();
      }
      for(NetAnt * a : ants){
        a->render();
      }
      if(borderRenderable != -1){
          warp::drawStaticRenderable(blockDirtRenderable);
          // warp::drawStaticRenderable(blockReinforcedRenderable);
          // warp::drawStaticRenderable(blockWaterRenderable);
          // warp::drawStaticRenderable(blockLavaRenderable);
          // warp::drawStaticRenderable(blockAbyssRenderable);

/*        warp::drawStaticRenderable(borderRenderable);
        for (int x = 0; x < blockCountWidth; x++) {
          for (int y = 0; y < blockCountHeight; y++) {
            warp::drawRect((x - blockCountWidth / 2.0) * blockSize, 
                (y - blockCountHeight / 2.0) * blockSize,
                blockSize,
                blockSize,
                0,
                worldColors[blocks[x][y]] 
                );
          }
        }*/
      }

      // render to fog of war buffer
      if (fogOfWarEnabled) {
        warp::setCurrentFbo(fogOfWarFboId);

        for (auto n : nodes) {
          n.second->renderVision();
        }

        for (auto a : antMap[playerId]) {
          dynamic_cast<Ant*>(a.second)->renderVision();
        }
        warp::clearCurrentFbo();
      }

      // draw hud
      warp::setRenderTarget(warp::HUD);

      // draw fog of war to screen
      if (fogOfWarEnabled) {
        warp::drawSprite(10 * warp::options::getAspectRatio(), 10, 20 * warp::options::getAspectRatio(), -20, 0,
            warp::fbo::getFboTextureId(fogOfWarFboId));
      }


      // draw debug text
      warp::drawRect(2, 19, 5, 4, 0, worldColorBlack);
      warp::drawString(1, 17.2, 0.5, "territories: " + std::to_string(numTerritories));
      warp::drawString(1, 17.8, 0.5, "borders: " + std::to_string(borders.size()));
      warp::drawString(1, 18.4, 0.5, "ants: " + std::to_string(getAntCounter()));
      warp::drawString(1, 19, 0.5, "idle: " + std::to_string(getNumIdleAnts()));
      warp::drawString(1, 19.6, 0.5, "jobs: " + std::to_string(tasks[ECONOMY].size()));


      // clear fog of war fbo
      if (fogOfWarEnabled) {
        warp::setCurrentFbo(fogOfWarFboId);
        warp::drawRect(10 * warp::options::getAspectRatio(), 10, 20 * warp::options::getAspectRatio(),
            20, 0, warp::Color(0, 0, 0, 1));
        warp::clearCurrentFbo();
      }
    }

    void updateWorld(float updateFactor){
      //      printf("num tasks: %d\n", tasks.size());
      for(NetAnt *a : ants){
        a->update(updateFactor);
        if(a->isDead()){
          deadAnts.push_back(a);
        }
      }
      for(NetAnt *a : deadAnts){
        killAnt(a);
      }
      deadAnts.clear();
      for(auto a : nodes){
        a.second->update(updateFactor);
      }
    }

    void addTask(Task *t){
      if(t == NULL){
        printf("ERROR while adding task: task is NULL\n");
      }
      else{
        tasks[t->getType()].push_back(t);
      }
    }

    bool compTasks(Task *t1, Task *t2){
      return (t1->getWalkingDistance() < t2->getWalkingDistance());
    }

    Task *getTask(Ant *a){
      if(a->getCurrentNode()->getNeighborCount() == 0){
        return NULL;
      }
      for(int i = 0; i < NUM_TASK_TYPES; i++){
        if(a->canDoTask(i)){
          for(Task *t : tasks[i]){
            t->updateWalkingDistance(a);
          }
          std::sort(tasks[i].begin(), tasks[i].end(), compTasks);
          for(unsigned int j = 0; j < tasks[i].size(); j++){
            Task *t = tasks[i][0];
            if(t->test(a)){ // check if the task can be done
              tasks[i].erase(tasks[i].begin());
              return t;
            }
            else{ // otherwise move it to the back of the vector
              std::rotate(tasks[i].begin(), tasks[i].begin() + 1, tasks[i].end());
            }
          }
        }
      }
      TaskWander *t = new TaskWander();
      if(t->test(a) == true){
        return t;
      }
      delete t;
      return NULL;
    }

    void deleteTask(Task *t){
      tasks[t->getType()].erase(std::remove(tasks[t->getType()].begin(), tasks[t->getType()].end(), t), tasks[t->getType()].end());

      delete t;
    }

    void spawnAnt(int x, int y, int strength){
      Node *n = getNodeAt(x, y);
      if(n != NULL){
        Ant *a = new Ant(x, y, lastAntId);
        a->setStrength(strength);
        a->setCurrentNode(n);
        a->setLifetime(options::getAntLifetime());
        ants.push_back(a);
        antMap[playerId][lastAntId] = a;
        lastAntId ++;
      }
    }

    void spawnAnt(Ant *a){
      ants.push_back(a);
      antMap[playerId][a->getId()] = a;
    }

    void spawnNetAnt(int player, int id, int x, int y){
      NetAnt *a = new NetAnt(x, y, 1, id, player);
      ants.push_back(a);
      antMap[player][id] = a;
    }

    void updateNetAnt(int player, int id, int x, int y, float dx, float dy){
      auto t = antMap[player].find(id);
      if(t != antMap[player].end()){
        t->second->setBaseX(x);
        t->second->setBaseY(y);
        t->second->setOffsetX(0);
        t->second->setOffsetY(0);
        t->second->setSpeedX(dx);
        t->second->setSpeedY(dy);
        t->second->recomputeMotionNoise();
      }

    }

    void killNetAnt(int player, int id){
      auto t = antMap[player].find(id);
      if(t != antMap[player].end()){
        t->second->onDeath();
        ants.erase(std::remove(ants.begin(), ants.end(), t->second), ants.end());
        antMap[player].erase(t);
        //t->second->die();
      }
    }


    void rebuildBorderRenderable(){
      //std::vector<Border*> borders = *ant::server::world::getBorders();
      if(borderRenderable != -1){
        warp::deleteStaticRenderable(borderRenderable);
        borderRenderable = -1;
      }
      float *vertices = new float[borders.size() * 4];
      for(unsigned int i = 0; i < borders.size(); i++){
        Border *b = borders[i];
        vertices[i * 4] = static_cast<float>(b->getX1());
        vertices[i * 4 + 1] = static_cast<float>(b->getY1());
        vertices[i * 4 + 2] = static_cast<float>(b->getX2());
        vertices[i * 4 + 3] = static_cast<float>(b->getY2());
      }

      borderRenderable = warp::createStaticLineRenderable(vertices, borders.size() * 2, 10, warp::Color(0.3, 0.3, 0.3, 1));
      delete[] vertices;

      // generate some blocks to render
      if (blocks == NULL) {
        blockCountWidth = 248;
        blockCountHeight = 248;
        blocks = new unsigned char*[blockCountWidth];
        for (unsigned int i = 0; i < blockCountWidth; i++) {
          blocks[i] = new unsigned char[blockCountHeight];
          std::memset(blocks[i], static_cast<char>(BlockType::DIRT), blockCountHeight);
          blocks[i][14] = static_cast<unsigned char>(BlockType::LAVA);
          blocks[i][24] = static_cast<unsigned char>(BlockType::WATER);
        }
      }
      vertices = new float[blockCountWidth * blockCountHeight * 8];

      //for (BlockType type = BlockType::DIRT; type < BlockType::ABYSS; type = static_cast<BlockType>(static_cast<int>(type) + 1)) {
        BlockType type = BlockType::DIRT;
        unsigned int numBlockType = 0;
        for (unsigned int x = 0; x < blockCountWidth; x++) {
          for (unsigned int y = 0; y < blockCountHeight; y++) {
            if (blocks[x][y] == static_cast<unsigned char>(type)) { 
              vertices[numBlockType * 8 + 0] = (x - blockCountWidth) * blockSize;
              vertices[numBlockType * 8 + 1] = (y - blockCountWidth) * blockSize;

              vertices[numBlockType * 8 + 2] = (x - blockCountWidth + 1) * blockSize;
              vertices[numBlockType * 8 + 3] = (y - blockCountWidth + 0) * blockSize;

              vertices[numBlockType * 8 + 4] = (x - blockCountWidth + 0) * blockSize;
              vertices[numBlockType * 8 + 5] = (y - blockCountWidth + 1) * blockSize;

              vertices[numBlockType * 8 + 6] = (x - blockCountWidth + 1) * blockSize;
              vertices[numBlockType * 8 + 7] = (y - blockCountWidth + 1) * blockSize;
              numBlockType ++;
            }
          }
        }
        switch (type) {
          case BlockType::EMPTY:
            // blockEmtpyRenderable = warp::createStaticRectRenderable(vertices, numBlockType * 4, colorDirt);
            break;
          case BlockType::DIRT:
            blockDirtRenderable = warp::createStaticRectRenderable(vertices, numBlockType * 4, colorDirt);
            break;
          case BlockType::REINFORCED:
            blockReinforcedRenderable = warp::createStaticRectRenderable(vertices, numBlockType * 4, colorDirt);
            break;
          case BlockType::WATER:
            blockWaterRenderable = warp::createStaticRectRenderable(vertices, numBlockType * 4, colorDirt);
            break;
          case BlockType::LAVA:
            blockLavaRenderable = warp::createStaticRectRenderable(vertices, numBlockType * 4, colorDirt);
            break;
          case BlockType::ABYSS:
            blockAbyssRenderable = warp::createStaticRectRenderable(vertices, numBlockType * 4, colorDirt);
            break;
        }
      //}
      delete[] vertices;
    }


    bool intersectsBorder(int x1, int y1, int x2, int y2){
      Border temp(x1, y1, x2, y2);
      for(Border *b : borders){
        if(b->intersectsWith(&temp)){

          return true;
        }
      }
      return false;
    }



    int countBorderIntersections(int x1, int y1, int x2, int y2){
      int i = 0;
      Border temp(x1, y1, x2, y2);
      for(Border *b : borders){
        if(b->intersectsWith(&temp)){
          i++;
        }
      }
      return i;

    }


    int *getClosestIntersection(int x1, int y1, int x2, int y2){
      int dist = INT_MAX;
      int *r = new int[2];
      int *i;
      Border temp(x1, y1, x2, y2);
      for(Border *b : borders){
        if(b->intersectsWith(&temp)){
          i = b->getIntersectionWith(&temp);
          if(std::hypot(i[0] - x1, i[1] - y1) < dist){
            r[0] = i[0];
            r[1] = i[1];
            dist = std::hypot(i[0] - x1, i[1] - y1);
          }

          delete[] i;
        }
      }
      return r;
    }

    void setFogOFWarEnabled(bool b) {
      fogOfWarEnabled = b;
    }

    bool isFogOfWarEnabled() {
      return fogOfWarEnabled;
    }

    void digTunnel(int x1, int y1, int x2, int y2, int width){

    }


    Node *getNodeAt(int x,int y, int tolerance){
      double dist = tolerance;
      Node *node = NULL;
      for(auto n : nodes){
        //if(hypot(x - n.second->getX(), y - n.second->getY()) < dist){
        if(std::hypot(x - n.second->getX(), y -  n.second->getY()) < dist){
          //dist = hypot(x - n.second->getX(), y - n.second->getY());
          dist = std::hypot(x - n.second->getX(), y - n.second->getY());
          node = n.second;
        }
      }
      return node;
      }


      Node *createNodeAt(int x, int y){
        Node *n = new Node(x, y, false);
        nodes[n->getId()] = n;
        return n;
      }

      void deleteNodeAt(int x, int y){
        Node *node = getNodeAt(x, y, 100);
        if(node != NULL){
          node->isolate();
          if(node->isDeletable()){
            nodes.erase(nodes.find(node->getId()));
            if(find(importantNodes.begin(), importantNodes.end(), node) != importantNodes.end()){
              importantNodes.erase(std::remove(importantNodes.begin(), importantNodes.end(), node), importantNodes.end());
            }
            delete node;
          }
        }
      }

      void registerHive(NodeHive *nh){
        hives.push_back(nh);
      }

      void setMainHive(NodeHive *nh){
        mainHive = nh;
        mainHive->setOwner(playerId);
      }

      void killAnt(NetAnt *a){
        a->onDeath();
        ants.erase(std::remove(ants.begin(), ants.end(), a), ants.end());
        auto it = antMap[playerId].find(a->getId());
        if(it != antMap[playerId].end()){
          antMap[playerId].erase(it);
        }
        delete a;
      }

      Node *getNodeById(int id){
        return nodes[id];
      }

      int getCenterX(){
        return centerX;
      }

      int getCenterY(){
        return centerY;
      }


      void cleanupWorld(){
        for(auto a : nodes){
          delete a.second;
        }
        for(NetAnt *a : ants){
          delete a;
        }
        for(Border *b : borders){
          delete b;
        }
        for(TunnelNode *n : tunnelNodes){
          delete n;
        }
        for(Tunnel *t : tunnels){
          delete t;
        }
        for(int i = 0; i < NUM_RESOURCES; i++){
          for(Task *t : tasks[i]){
            delete t;
          }
          tasks[i].clear();
        }

        nodes.clear();
        importantNodes.clear();
        hives.clear();
        ants.clear();
        borders.clear();
        tunnelNodes.clear();
        tunnels.clear();
      }


      NodeHive *getMainHive(){
        return mainHive;
      }


      void buildBorder(int id, int x1, int y1, int x2, int y2){
        Border *b = new Border(id, x1, y1, x2, y2);
        borders.push_back(b);
        rebuildBorderRenderable();
      }

      void deleteBorder(int id){
        borders.erase(std::remove_if(borders.begin(), borders.end(), [id](Border *b){return b->getId() == id;}), borders.end());
      }

      std::vector<Border*>* getBorders(){
        return &borders;
      }

      std::vector<Node*>* getImportantNodes(){
        return &importantNodes;
      }

      std::vector<NodeHive*>* getHives(){
        return &hives;
      }

      void addNode(Node *n){
        nodes[n->getId()] = n;
        if(n->isInteresting()){
          importantNodes.push_back(n);
        }
      }

      void setNumTerritories(int i){
        numTerritories = i;
      }
      int getNumTerritories(){
        return numTerritories;
      }

      void setPlayerId(int id){
        playerId = id;
      }

      int getPlayerId(){
        return playerId;
      }

      void newPlayer(int id){
        antMap.insert(std::pair<int, std::map<int, NetAnt*>>(id, std::map<int, NetAnt*>()));
      }

      std::map<int, std::map<int, NetAnt*>> *getAntMap(){
        return &antMap;
      }

      int getNewAntId(){
        lastAntId ++;
        return lastAntId - 1;
      }
    }
  }

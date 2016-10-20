#include "NodeHive.h"
#include "warp/Renderer.h"
#include "WorldClient.h"
#include "TaskConquer.h"
#include "Options.h"
#include "Client.h"
#include "Server.h"
#include <cstring>
#include <cmath>

namespace ant{

  warp::Color hiveColor(1, 0, 0, 1);  

  // controls spead of heartbeat TODO(florian): make this dynamic and hive based
  const int bpm = 90;
  const int heartbeatCycle = 60000 / bpm;

  NodeHive::NodeHive(int x, int y, int owner, bool onServer) : Node(x, y, onServer), spawnCounter(1000), owner(owner), freeJobs(0), numWorkers(0), pulseTimer(0){
    setInteresting(true);
    setDeletable(false);
    setResourceMaxAll(10000);
    setResourceMaxTotal(500);
    storeResource(0, 100);  
  }

  NodeHive::NodeHive(int x, int y, int id, int owner, bool onServer) : Node(x, y, id, onServer), spawnCounter(1000), owner(owner), freeJobs(0), numWorkers(0), pulseTimer(0){
    setInteresting(true);
    setDeletable(false);
    setResourceMaxAll(10000);
    setResourceMaxTotal(500);
    storeResource(0, 100);  
  }

  NodeHive::~NodeHive(){

  }

  void NodeHive::render(){
    //  warp::drawRect(getX(), getY(), 2500, 2500, 0, hiveColor);
    Node::render();
   
    //    warp::drawCircle(getX(), getY(), 150 * getPercentageFullTotal() + 100, hiveColor);
    warp::drawSprite(getX(), getY(), 300 * getPercentageFullTotal() + 200 + 50 * pulseRenderValue, 300 * getPercentageFullTotal() + 200 + 50 * pulseRenderValue, 0, "steampunk_heart1");

  }

  void NodeHive::produceAnt(){
    int effects[options::getNumResourceEffects()];
    std::memset(effects, 0, sizeof(int) * options::getNumResourceEffects());
    bool usedResources[NUM_RESOURCES];
    int numResources = 0;
    for(int i = 0; i < NUM_RESOURCES; i++){
      usedResources[i] = false;
    }
    int cost = options::getAntCost();
    for(int i = 0; i < NUM_RESOURCES; i++){
      if(!usedResources[i]){
        if(hasResource(i, cost)){
          for(int j = 0; j < options::getNumResourceEffects(); j++){
            effects[j] += options::getRessourceEffectMatrix(i)[j];
          }
          usedResources[i] = true;
          numResources ++;
          if(numResources > 1){
            cost -= options::getCombinedRessourceCostReduction();
            i = 0;
          }
        }
      }
    }
    if(numResources > 0){
      for(int i = 0; i < NUM_RESOURCES; i++){
        getResources(i, cost);
      }
      Ant *a = new Ant(getX(), getY(), world::getNewAntId());
      a->setCurrentNode(this);

      /*printf("current effect matrix: %d %d %d %d %d %d %d %d %d %d %d\n", effects[0], effects[1], effects[2], effects[3], effects[4], effects[5], effects[6], effects[7], effects[8], effects[9], effects[10]); TODO decide what to do with the effect code

        spawnCounter -= effects[0];

        a->setLifetime(options::getAntLifetime() + effects[1]);
        a->setStrength(options::getAntStrength() + effects[2]);
        a->setAbility(CONQUER, effects[3] == 0);
        a->setSpeed(options::getAntSpeed() + effects[4]);
        a->setAbility(CONSTRUCTION, effects[5] == 0);
        a->setRallySpeedBonus(effects[6]);
        a->setConqueringPower(options::getAntConqueringPower() + (effects[7] > 0 ? options::getConqueringThreshold() : 0));
        a->setAbility(ECONOMY, effects[8] == 0);
        a->setHarvestTimeReduction(effects[9]);
        a->setAbility(MILITARY, effects[10] == 0);*/
      world::spawnAnt(a); 
    }


  }


  void NodeHive::updateConqueringProgressBy(int player, int amount){
    if(conqueringProcess.find(player) == conqueringProcess.end()){
      conqueringProcess[player] = 0;
    }
    conqueringProcess[player] += amount;
    for(auto a : conqueringProcess){
      if(a.first != player){
        conqueringProcess[a.first] -= amount;
        if(conqueringProcess[a.first] < 0){
          conqueringProcess[a.first] = 0;
        }
      }
    }
    if(conqueringProcess[player] > options::getConqueringThreshold()){
      conqueringProcess[player] = options::getConqueringThreshold();
      if(owner != player){
        owner = player;
        unsigned char data[8];
        writeIntToBuffer(getId(), data, 0);
        writeIntToBuffer(player, data, 4);
        server::broadcastPacket(NODE_HIVE_CONQUERED, data, 8);
      }
    }
  }

  void NodeHive::update(float updateFactor){
    if(owner == world::getPlayerId() && !onServer){
      if(spawnCounter > 0){
        spawnCounter -= updateFactor * 1000;
      }
      else{
        spawnCounter = 0;
        if(getAntCounter() < 5000){
          produceAnt();
        }
        spawnCounter += options::getAntSpawnCooldown();
      } 
    }
    Node::update(updateFactor);
    pulseTimer += updateFactor * 1000;
    pulseValue = pulseTimer > heartbeatCycle / 2 ? 0.3398 : (1 - pow(pulseTimer / (heartbeatCycle / 4.0) - 1, 2) + 0.3398);
    pulseRenderValue = ((pulseTimer + heartbeatCycle / 4) % heartbeatCycle) > heartbeatCycle / 2 ? 0.3398 : (1 - pow(((pulseTimer + heartbeatCycle / 4) % heartbeatCycle) / (heartbeatCycle / 4.0) - 1, 2) + 0.3398);
    //pulseValue = pulseTimer < 500 ? 1 - pow(pulseTimer / 250.0 - 1, 2) : pulseTimer > 750 && pulseTimer < 1250 ? 1 - pow((pulseTimer - 750) / 250.0 - 1, 2) : 0;
    pulseTimer %= heartbeatCycle;
  }


  int NodeHive::getCreationPacketSize(){
    return 12;
  }


  void NodeHive::writeCreationPacket(unsigned char *buff){
    writeIntToBuffer(getX(), buff, 0);
    writeIntToBuffer(getY(), buff, 4);
    writeIntToBuffer(getId(), buff, 8); 
  }

  void NodeHive::updatePathTo(Node *goal, int dist){
    Node::updatePathTo(goal, dist);
    if(goal == world::getMainHive()){
      updateTaskState();
    }
  }

  void NodeHive::updateTaskState(){
    if(freeJobs < 1 && numWorkers < options::getConqueringThreshold() && owner != world::getPlayerId() &&  getDirectionTo(world::getMainHive()) != NULL){
      TaskConquer *t = new TaskConquer(this);
      t->setStart(this);
      world::addTask(t);  
    }
  }

  void NodeHive::onTaskAccepted(Task *t){
    if(t->getType() == CONQUER){
      freeJobs --;
      numWorkers ++;
      updateTaskState();
    }

    Node::onTaskAccepted(t);
  }


  void NodeHive::onTaskAborted(Task *t){
    if(t->getType() == CONQUER ){
      if(t->getState() == 0){
        freeJobs ++;
      }
      numWorkers --;
    }
    Node::onTaskAborted(t);
  }


  void NodeHive::onTaskFinished(Task *t){
    if(t->getType() == CONQUER){
      unsigned char data[9];
      writeIntToBuffer(getId(), data, 0);
      writeIntToBuffer(world::getPlayerId(), data, 4);
      data[8] = t->getServant()->getConqueringPower(); 
      client::sendPacket(NODE_HIVE_CONQUERING, data, 9);
      numWorkers --;
    }
    Node::onTaskFinished(t);
  }


  PACKET_TYPE NodeHive::getCreationPacketType(){
    return NEW_NODE_HIVE;
  }

  NodeHive *createNodeHiveFromPacket(unsigned char *buff){
    return new NodeHive(readIntFromBuffer(buff, 0), readIntFromBuffer(buff, 4), readIntFromBuffer(buff, 8), false, false);
  }

  void NodeHive::setOwner(int i){
    owner = i;
  }
  float NodeHive::getPulseStrength() {
    return pulseValue;
  }
}

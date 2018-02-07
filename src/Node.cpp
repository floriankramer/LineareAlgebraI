#include "Node.h"
#include "warp/Renderer.h"
#include <algorithm>
#include "WorldClient.h"
#include "Mathhelper.h"
#include <cstdio>
#include <cmath>
#include <climits>
#include "TaskFight.h"
#include "Client.h"
#include "Ant.h"

namespace ant{

  warp::Color nodeColor(0, 0, 1, 1);
  warp::Color nodeColorCombat(1, 0, 0, 1);
  warp::Color nodeConnectionColor(0, 1, 0, 1);

  unsigned int nodeIdTracker = 0;
  unsigned int localNodeIdOffset = 65536;


  Node::Node(bool onServer) : onServer(onServer), x(0), y(0), interesting(false), deletable(true), id(nodeIdTracker), connectionUpdateCooldown(0), freeTasks(0), combatPoint(false){
    nodeIdTracker ++;
    if(!onServer){
      id += localNodeIdOffset;
    }
  }

  Node::Node(int x, int y, bool onServer) : onServer(onServer), x(x), y(y), interesting(false), deletable(true), id(nodeIdTracker), connectionUpdateCooldown(0), freeTasks(0), combatPoint(false){
    nodeIdTracker ++;
    if(!onServer){
      id += localNodeIdOffset;
    }
  }

  Node::Node(int x, int y, unsigned int id, bool onServer) : onServer(onServer), x(x), y(y), interesting(false), deletable(true), id(id), connectionUpdateCooldown(0), freeTasks(0), combatPoint(false){
    if(id > nodeIdTracker){
      nodeIdTracker = id + 1;
    } 
  }


  Node::~Node(){

  }


  void Node::render(){
    warp::drawCircle(x, y, 25, nodeColor);
    if(combatPoint){
      warp::drawCircle(x, y, 20, nodeColorCombat);
    }
    //warp::drawString(x, y, 500, std::to_string(id), nodeConnectionColor);
    for(Node *n : neighbors){
      warp::drawLine(x, y, n->getX(), n->getY(), 5 * pow(connections[n->getId()]->speedMultiplier, 2), nodeConnectionColor);
    }
    //  int i = 0;
    //  for(auto &iter : directions){
    //    warp::drawString(x + 300, y + i * 500, 500, std::to_string(iter.second.dist), nodeColor);
    //    i++;
    //  }
  }


  void Node::renderVision() {
    if (getDistanceTo(world::getMainHive()) < INT_MAX) {
      warp::drawCircle(x, y, 1000, warp::Color(1, 1, 1, 0)); 
    } 
  }

  void Node::connectTo(Node *n){
    if(find(neighbors.begin(), neighbors.end(), n) == neighbors.end()){
      neighbors.push_back(n);
      if(connections.find(n->getId()) == connections.end()){
        NodeConnection *nc = new NodeConnection();
        nc->creatorId = getId();
        connections[n->getId()] = nc; 
        n->setConnectionInfoTo(getId(), nc);
        nc->users = 2;
        nc->length = std::hypot(n->getX() - getX(), n->getY() - getY());
      }
      n->connectTo(this);
      for(auto &iter : directions){
        updatePathTo(world::getNodeById(iter.first), iter.second.dist);
      }
      if(interesting){
        nodeDirection n;
        n.next = this;
        n.dist = 0;
        directions[id] = n;
        updatePathTo(this, 0);
      }


    }
  }

  void Node::setConnectionInfoTo(int id, NodeConnection *n){
    connections[id] = n;
  }


  Node *Node::getDirectionTo(Node *n){
    if(directions.find(n->getId()) != directions.end()){
      return directions[n->getId()].next;
    }
    else if(std::find(neighbors.begin(), neighbors.end(), n) != neighbors.end()){
      return n;
    }
    return NULL;
  }


  void Node::updatePathTo(Node *goal, int dist){
    //printf("%d, %d, %d \n", goal->getId(), getId(), dist);
    for(Node *n : neighbors){ 
      int newdist = dist + getDistanceTo(n);  
      if(n->getDistanceTo(goal) >= newdist){  
        n->setPathTo(goal, this, newdist);
        n->updatePathTo(goal, newdist);
      }
    } 
  }

  void Node::attachAnt(NetAnt *a){
    antsAttached.push_back(a);
  }

  void Node::deattachAnt(NetAnt *a){
    antsAttached.erase(std::remove(antsAttached.begin(), antsAttached.end(), a), antsAttached.end());
  }

  void Node::setPathTo(Node *goal, Node *nextNode, int dist){
    if(directions.find(goal->id) != directions.end()){
      directions[goal->id].next = nextNode;
      directions[goal->id].dist = dist;
    }
    else{
      nodeDirection n;
      n.next = nextNode;
      n.dist = dist;
      directions[goal->id] = n;
    }
  }

  int Node::getDistanceTo(Node *n){
    if(find(neighbors.begin(), neighbors.end(), n) != neighbors.end()){
      return static_cast<int>(std::hypot(n->getX() - x, n->getY() - y));
    }
    else if(n == this){
      return 0;
    }
    else if(n == NULL){
      return INT_MAX;
    }
    else if(n->interesting){
      if(directions.find(n->id) != directions.end()){
        return directions[n->id].dist;

      } 
    }

    return INT_MAX;
  }

  void Node::removeDirection(Node *goal){
    if(directions.find(goal->getId()) != directions.end()){
      int dist = getDistanceTo(goal);
      for(Node *n : neighbors){
        if(n->getDistanceTo(goal) > dist){
          n->removeDirection(goal);
        }
      }
      directions.erase(directions.find(goal->getId()));
    }
  }

  bool Node::searchForDirection(Node *goal, std::vector<Node*> *searched){
    int dist = getDistanceTo(goal);
    searched->push_back(this);
    for(Node *n : neighbors){
      if(std::find(searched->begin(), searched->end(), n) == searched->end()){
        if(n->getDistanceTo(goal) < dist){
          n->updatePathTo(goal, n->getDistanceTo(goal));
          return true;  
        }
        else{
          if(n->searchForDirection(goal, searched)){
            return true;
          }
        }
      }
    }
    return false;

  }

  void Node::isolate(){
    for(auto a : connections){
      a.second->users = 1;
    }
    connections.clear();
    for(Node *n : neighbors){
      n->disconnectFrom(this);
    }

    if(neighbors.size() > 1 || interesting){
      for(auto &a : directions){
        Node *goal = world::getNodeById(a.first);
        int dist = getDistanceTo(goal);
        for(Node *n : neighbors){ 
          if(n->getDistanceTo(goal) > dist){
            n->removeDirection(goal);
          }
        }
        for(Node *n : neighbors){
          if(n->getDistanceTo(goal) == INT_MAX){
            std::vector<Node *> searched;
            n->searchForDirection(goal, &searched);
          } 
        }
        //  world::getNodeById(a.first)->updatePathTo(world::getNodeById(a.first), 0);  
      }
    }
    directions.clear();
    neighbors.clear();

    for(Ant *a : std::vector<Ant*>(antsComing)){ // TODO think about more elegant solution to not editing the vector while iterating
      a->onDeleteGoal();
    }
    for(Ant *a : std::vector<Ant*>(antsGoing)){
      a->onDeleteCurrent();
    }
    antsGoing.clear();
  }

  void Node::disconnectFrom(Node *n){
    neighbors.erase(remove(neighbors.begin(), neighbors.end(), n), neighbors.end());
    if(connections.find(n->getId()) != connections.end()){
      if(connections[n->getId()]->users == 1){
        delete connections[n->getId()];
      }
      connections.erase(connections.find(n->getId()));
    }
  }

  Node *Node::getNeighbor(int i){
    return neighbors[i];
  }

  int Node::getNeighborCount(){
    return neighbors.size();
  }

  int Node::getX(){
    return x;
  }

  int Node::getY(){
    return y;
  }

  void Node::setInteresting(bool b){
    interesting = b;
  }

  bool Node::isInteresting(){
    return interesting;
  }

  unsigned int Node::getId(){
    return id;
  }

  bool Node::isDeletable(){
    return deletable;
  }

  void Node::setDeletable(bool b){
    deletable = b;
  }


  void Node::update(float updateFactor){
    if(!onServer){
      if(connectionUpdateCooldown > 0){
        connectionUpdateCooldown -= 1000 * updateFactor;
      }
      else{
        for(auto a : connections){
          if(a.second->creatorId == getId()){
            float newValue = 2 - (2 - 1) * pow(2.71828182846, -0.015 * (a.second->useCount * a.second->length));//0.05 * a.second->useCount * (2 - a.second->useCount); //logistic growth 
            a.second->speedMultiplier = (a.second->speedMultiplier * 0.9 + newValue * 0.1); //smooth out change
            a.second->useCount = 0;
          }
          else{
            if(static_cast<int>(a.second->creatorId) != a.first){
              printf("ID ERROR: connection is not getting updated\n");
            }
          }
        }
        connectionUpdateCooldown = 500;
      }
    }
    else{
      if(combatPoint){
        for(NetAnt *a : antsAttached){
          if(rand() % 120 == 0){
            if(std::hypot(x - a->getX(), y - a->getY()) < 3){
              for(NetAnt *a2 : antsAttached){
                if(a2->getPlayerId() != a->getPlayerId()){
                  a2->attack(a);
                  a->attack(a2);
                }
              }
            }
          }
        }
      }

    }
  }

  float Node::getSpeedMultiplierTo(Node *n){
    return connections[n->getId()]->speedMultiplier;
  }

  void Node::addGoingTo(Ant *a){
    if(connections.find(a->getCurrentNode()->getId()) != connections.end()){
      connections[a->getCurrentNode()->getId()]->useCount ++; 
    }
    antsComing.push_back(a);  
  }

  void Node::removeGoingTo(Ant *a){
    antsComing.erase(std::remove(antsComing.begin(), antsComing.end(), a), antsComing.end());
  }

  void Node::addComingFrom(Ant *a){ 
    antsGoing.push_back(a); 
  }

  void Node::removeComingFrom(Ant *a){
    antsGoing.erase(std::remove(antsGoing.begin(), antsGoing.end(), a), antsGoing.end());
  }


  void Node::setCombatPoint(bool b){
    combatPoint = b;
    unsigned char data[5];
    data[4] = b ? 1 : 0;
    writeIntToBuffer(getId(), data, 0);
    client::sendPacket(SET_NODE_COMBAT_STATE, data, 5);
    updateCombatTask();
  }

  bool Node::isCombatPoint(){
    return combatPoint;
  }

  void Node::updateCombatTask(){
    if(combatPoint && freeTasks < 1){
      TaskFight *t = new TaskFight(this);
      t->setStart(this);
      world::addTask(t);
      freeTasks ++;
    }
    else if(!combatPoint && freeTasks > 0){
      //TODO have task provider keep reference to tasks to make them deletable
    }

  }

  void Node::onTaskAccepted(Task *t){
    if(t->getType() == MILITARY){
      freeTasks --;
      updateCombatTask();
    }

  }


  void Node::onTaskAborted(Task *t){
    if(t->getType() == MILITARY){
      if(t->getState() == 0){
        freeTasks ++;
      }
    }


  }

  void Node::setX(int x) {
    this->x = x; 
  }

  void Node::setY(int y) {
    this->y = y; 
  }

  void Node::onTaskFinished(Task *t){

  }


  bool Node::canBuildPathFrom() {
    return true;
  }
}

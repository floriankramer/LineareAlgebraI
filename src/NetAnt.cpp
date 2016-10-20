#include "NetAnt.h"

#include <cmath>
#include <cstdlib>

#include "warp/Renderer.h"

#include "NetworkHelper.h"
#include "Server.h"
#include "Client.h"
#include "WorldClient.h"
#include "Node.h"


namespace ant{

  unsigned int motionNoiseSeed;

  warp::Color colorAnt(0, 0, 0, 1);

  NetAnt::NetAnt(int id, int playerId) :
    current(NULL),
    x(0),
    y(0),
    baseX(0),
    baseY(0),
    speedX(0),
    speedY(0),
    perpX(0),
    perpY(0),
    motionNoisePhase(0),
    offsetX(0),
    offsetY(0),
    strength(1),
    dead(false),
    id(id),
    playerId(playerId){
    }

  NetAnt::NetAnt(int x, int y, int strength, int id, int playerId) :
    current(NULL),
    x(x),
    y(x),
    baseX(x),
    baseY(y),
    speedX(0),
    speedY(0),
    perpX(0),
    perpY(0),
    motionNoisePhase(0),
    offsetX(0),
    offsetY(0),
    strength(strength),
    dead(false),
    id(id),
    playerId(playerId){
    }

  NetAnt:: ~NetAnt(){ 
  }

  void NetAnt::render(){
//    warp::drawCircle(getVisualX() - 8, getVisualY() - 8, 8, colorAnt);
    warp::drawCircle(getVisualX(), getVisualY(), 8, colorAnt);
//    warp::drawCircle(getVisualX() + 8, getVisualY() + 8, 8, colorAnt);
  }

  void NetAnt::setSpeedX(float speedX){
    this->speedX = speedX;
  }

  void NetAnt::setSpeedY(float speedY){
    this->speedY = speedY;
  }

  void NetAnt::update(float updateFactor){
    //printf("moving at: %f, %f with updateFactor %f\n", speedX, speedY, updateFactor);
    // TODO change hive to hive of actual player
    offsetX += updateFactor * (speedX * 0.5 + 0.5 * speedX * world::getMainHive()->getPulseStrength());
    offsetY += updateFactor * (speedY * 0.5 + 0.5 * speedY * world::getMainHive()->getPulseStrength());

    if (fabs(speedX) > 0.0001 || fabs(speedY) > 0.0001) {
      motionNoisePhase += updateFactor * 5;
    } else {
      motionNoisePhase = 0;
    }

    x = offsetX + baseX; // + sin(motionNoisePhase) * perpX * 8;
    y = offsetY + baseY; // + sin(motionNoisePhase) * perpY * 8;

    //if(abs(x - goalX) < 2 * speedX && abs(y - goalY < speedY)){

    //}
  }

  void NetAnt::recomputeMotionNoise() {
    float s = hypot(speedX, speedY);
    if (s > 0.000001) {
      perpX = speedY / s;
      perpY = -speedX / s; 
    } else {
      perpX = 0;
      perpY = 0;
    }
    motionNoisePhase = (rand_r(&motionNoiseSeed) % 31415) / 5000.0;
  }


  int NetAnt::getX(){
    return x;
  }

  int NetAnt::getY(){
    return y;
  }

  float NetAnt::getVisualX() {
    return x + sin(motionNoisePhase) * perpX * 8;
  }

  float NetAnt::getVisualY() {
    return y + sin(motionNoisePhase) * perpY * 8;
  }

  void NetAnt::setBaseX( int x){
    this->baseX = x;
  }

  void NetAnt::setBaseY( int y){
    this->baseY = y;
  }
  float NetAnt::getOffsetX(){
    return offsetX;
  }

  float NetAnt::getOffsetY(){
    return offsetY;
  }

  void NetAnt::setCurrentNode(Node *n){
    if(current != NULL){
      current->deattachAnt(this);
    }
    current = n;
    if(current != NULL){
      current->attachAnt(this);
    }

  }

  void NetAnt::setOffsetX(float x){
    this->offsetX = x;
  }

  void NetAnt::setOffsetY(float y){
    this->offsetY = y;
  }

  bool NetAnt::isDead(){
    return dead;
  }

  void NetAnt::die(){
    dead = true;
  }

  int NetAnt::getId(){
    return id;
  }

  int NetAnt::getStrength(){
    return strength;
  }

  void NetAnt::setStrength(int i){
    strength = i;
    if(strength <= 0){
      die();
    }
  }

  int NetAnt::getPlayerId(){
    return playerId;
  }

  void NetAnt::attack(NetAnt *a){
    a->setStrength(a->getStrength() - strength);
  }

  void NetAnt::sendCreate(){
    //    printf("sending ant create at: %d, %d\n", x, y);
    unsigned char data[17];
    writeIntToBuffer(world::getPlayerId(), data, 0);
    writeIntToBuffer(id, data, 4);

    writeIntToBuffer(x, data, 8);
    writeIntToBuffer(y, data, 12);
    data[16] = strength;

    if(client::isConnected()){
      client::sendPacket(NEW_ANT, data, 17);
    }

  }

  void NetAnt::sendSync(){
    unsigned char data[28];
    writeIntToBuffer(world::getPlayerId(), data, 0);
    writeIntToBuffer(getId(), data, 4);
    writeIntToBuffer(getX(), data, 8);
    writeIntToBuffer(getY(), data, 12);
    writeFloatToBuffer(speedX, data, 16);
    writeFloatToBuffer(speedY, data, 20);
    if(current != NULL){
      writeIntToBuffer(current->getId(), data, 24);
    }
    else{
      writeIntToBuffer(-1, data, 24);
    }
    if(client::isConnected()){
      client::sendPacket(ANT_MOTION, data, 28);
    }

  }



  void NetAnt::sendDeath(){
    unsigned char data[8];
    writeIntToBuffer(world::getPlayerId(), data, 0);
    writeIntToBuffer(id, data, 4);
    if(client::isConnected()){
      client::sendPacket(ANT_DEATH, data, 16);
    }

  }

}


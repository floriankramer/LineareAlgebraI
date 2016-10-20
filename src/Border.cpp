#include "Border.h"
#include "warp/Renderer.h"
#include "Mathhelper.h"
#include "warp/CollisionHandler.h"

namespace ant{

  warp::Color borderColor(0.3, 0.3, 0.3, 1);

  int borderIdCounter = 0;

  Border::Border( int startX,  int startY,  int stopX,  int stopY) : id(borderIdCounter), x1(startX), y1(startY), x2(stopX), y2(stopY){
    borderIdCounter ++; 
  }

  Border::Border(int id, int startX,  int startY,  int stopX,  int stopY) : id(id), x1(startX), y1(startY), x2(stopX), y2(stopY){

  }

  Border::~Border(){

  }

  int Border::getId(){
    return id;
  }

  void Border::render(){
    warp::drawLine(x1, y1, x2, y2, 10, borderColor);
  }


  bool Border::intersectsWith(Border *b){
    return warp::linesIntersect(x1, x2, y1, y2, b->x1, b->x2, b->y1, b->y2, 0.01);
    double v1x = (x2 - x1), v1y = (y2 - y1), v2x = (b->x2 - b->x1), v2y = (b->y2 - b->y1); //define direction vectors with length of line segment
    double s = (- v1x * (b->y1 - y1) + v1y * (b->x1 - x1)) / (v1x * v2y - v1y * v2x); //calculate intersection point variable for first line segment
    double t = (- v2x * (y1 - b->y1) + v2y * (x1 - b->x1)) / (v2x * v1y - v2y * v1x); //calculate intersection point variable for second line segment
    return s >= 0 && s <= 1 && t >= 0 && t <= 1; //s and t are only defined in [0;1], so only if they are in this range there is a solution

  }


  int* Border::getIntersectionWith(Border *b){
    int *i = new  int[2]; 
    float *f = warp::getLineIntersection(x1, x2, y1, y2, b->x1, b->x2, b->y1, b->y2, 0.01);
    i[0] = f[0];
    i[1] = f[1];
    delete[] f;
    return i;
    i[0] = i[1] = -1;
    float v1x = x2 - x1, v1y = y2 - y1, v2x = b->x2 - b->x1, v2y = b->y2 - b->y1; //define direction vectors with length of line segment
    double s = (- v1x * (b->y1 - y1) + v1y * (b->x1 - x1)) / (v1x * v2y - v1y * v2x); //calculate intersection point variable for first line segment
    double t = (- v2x * (y1 - b->y1) + v2y * (x1 - b->x1)) / (v2x * v1y - v2y * v1x); //calculate intersection point variable for second line segment
    if(s >= 0 && s <= 1 && t >= 0 && t <= 1){ //s and t are only defined in [0;1], so only if they are in this range there is a solution
      i[0] = x1 + t * v1x;
      i[1] = y1 + t * v1y;
      return i;
    }
    return i;

  }


  int Border::getX1(){
    return x1;
  }

  int Border::getX2(){
    return x2;
  }

  int Border::getY1(){
    return y1;
  }

  int Border::getY2(){
    return y2;
  }

  void Border::setX1(int x1){
    this->x1 = x1;
  }


  void Border::setX2(int x2){
    this->x2 = x2;
  }


  void Border::setY1(int y1){
    this->y1 = y1;
  }


  void Border::setY2(int y2){
    this->y2 = y2;
  }

}

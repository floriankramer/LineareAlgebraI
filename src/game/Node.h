/*
 * Node.h
 *
 *  Created on: Feb 9, 2015
 *      Author: dwarf
 */

#ifndef GAME_NODE_H_
#define GAME_NODE_H_


#include <vector>
#include "Entity.h"
#include "map"
#include "CollisionHandler.h"
#include <utility>

namespace game {

enum NodeDataType{
	INT, FLOAT, BOOL
};

struct HandleData{

	HandleData(){
		i = 0;
		b = false;
		f= 0;
	}

	HandleData(int i){
		this->i = i;
		b = i > 0;
		f = i;
	}

	HandleData(bool b){
		i = b;
		this->b = b;
		f = b;
	}

	HandleData(float f){
		i = f;
		b = f > 0;
		this->f = f;
	}

	int i;
	float f;
	bool b;
};


enum ActivationType{
	COLLISION,
	INTERACTION
};

//template<class T>
//struct ConnectionPoint{
//	ConnectionPoint(T* n, int h){
//		node = n;
//		handleId = h;
//	}
//
//	T *node;
//	int handleId;
//};

class Node {
public:
	Node();
	Node(float x, float y, float width, float height);
	virtual ~Node();
	virtual void activate(ActivationType a, void * data = NULL);
	virtual void onInputChanged(int handleId);
	virtual void render(bool editMode, float updateFactor);
	virtual void doGameUpdate(float updateFactor){}

	void renderConnections(float updateFactor);


	float getX();
	float getY();
	float getXCenter();
	float getYCenter();
	float getWidth();
	float getHeight();

	void setXCenter(float);
	void setYCenter(float);
	void setWidth(float);
	void setHeight(float);

	virtual void move(float updateFactor);

	bool doesCollide();
	bool doesMove();

	float getGoalX();
	float getGoalY();
	float getInitX();
	float getInitY();
	int getDirection();
	void setGoalX(float x);
	void setGoalY(float y);
	void setInitX(float x);
	void setInitY(float y);
	void setDirection(int i);
	void recalculateMotionData();
	void setSpeed(float s);
	int getActivation();
	int getUniqueId();
	void setUniqueId(int i);
	uint8_t getType();

	int getIngoingCount();

	physics::Rect getShape();

	float getSpeedX();
	float getSpeedY();

	float getMotionX();
	float getMotionY();

	int getInputHandleAt(float x, float y);
	int getOutputHandleAt(float x, float y);

	void connectOutputTo(int outputId, Node *n, int inputId);
	void removeIncomingConnection(int handleId);
	void removeOutgoingConnection(int handleId);

	void setIncoming(int handleId, Node *n, int outgoingHandle);

	float getInputHandleX(int handleId);
	float getInputHandleY(int handleId);


	HandleData getOutputHandleData(int handleId);
	HandleData getInputHandleData(int handleId);
	bool hasHandleData(int inputHandleId);

	int writeIngoingConections(char* buffer, int offset);
	int readIngoingConections(char* buffer, int offset, int uidOffset);
	void resolveIngoingConnections();

	void removeConnectionsTo(Node *n);

	bool isOpaque();

protected:
	void setCollides(bool);
	void setMoves(bool);
	void setType(uint8_t i);

	void addInput(int id, NodeDataType t);
	void addOutput(int id, NodeDataType t);
	void removeInput(int i);
	void removeOutput(int i);

	void removeOutgoingFrom(int handleId);
	void removeIncomingFrom(int handleId);

	void setHandleData(int id, HandleData d);
	void setOpaque(bool b);

private:

	std::map<int, std::pair<int, int>> tempIncomingConnections;

	std::map<int, NodeDataType> inputs;
	std::map<int, NodeDataType> outputs;

	std::map<int, std::pair<Node *, int>> outgoingConnections;
	std::map<int, std::pair<Node *, int>> incomingConnections;

	std::map<int, HandleData> handleData;

	float x, y, lastX, lastY;
	float width, height;
	int uniqueId;
	bool collides, moves;
	float initX, initY, goalX, goalY;
	int direction;
	float speedX, speedY;
	float speed;
	int activation;
	uint8_t type;
	//std::map<int, bool> inputs;
	physics::Rect shape;
	bool opaque;
};

void setUniqueIdCounter(int i);
int getUniqueIdCounter();


} /* namespace gui */


#endif /* GAME_NODE_H_ */

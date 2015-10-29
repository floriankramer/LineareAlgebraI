/*
 * Node.cpp
 *
 *  Created on: Feb 9, 2015
 *      Author: dwarf
 */

#include <Node.h>
#include "Renderer.h"
#include "Physics.h"
#include "World.h"
#include <cmath>
#include "Game.h"

namespace game {


render::Color colorINT(0, 1, 0);
render::Color colorFLOAT(.6, 0, 0.7);
render::Color colorBOOL(1, 0, 0);

render::Color colorConnection(0, 0, 1);
render::Color colorDirection(1, 0, 1);

int uidCounter = 0;
const float handleSize = 0.6;
const float handleSpace = 0.8;

void setUniqueIdCounter(int i){
	uidCounter = i;
}

int getUniqueIdCounter(){
	return uidCounter;
}

Node::Node() :
		x(0),
		y(0),
		lastX(0),
		lastY(0),
		width(0),
		height(0),
		uniqueId(uidCounter),
		collides(false),
		moves(false),
		initX(0),
		initY(0),
		goalX(0),
		goalY(0),
		direction(0),
		speedX(0),
		speedY(0),
		speed(0.1),
		activation (0),
		type(0),
		opaque(false)
{

	uidCounter ++;
}

Node::Node(float x, float y, float width, float height) :
				x(x),
				y(y),
				lastX(0),
				lastY(0),
				width(width),
				height(height),
				uniqueId(uidCounter),
				collides(false),
				moves(false),
				initX(0),
				initY(0),
				goalX(0),
				goalY(0),
				direction(0),
				speedX(0),
				speedY(0),
				speed(0.1),
				activation (0),
				type(0),
				opaque(false)
{
	uidCounter ++; //TODO should maybe be removed
//	this->x = x;
//	this->y = y;
//	this->width = width;
//	this->height = height;
//	uniqueId = 0;
//	collides = false;
//	moves = false;
//	initX = 0;
//	initY = 0;
//	goalX = 0;
//	goalY = 0;
//	direction = 0;
//	speedX = 0;
//	speedY = 0;
//	speed = 0.1;
//	activation = 0;
//	type = 0;
//	lastX = 0;
//	lastY = 0;
//	opaque = false;
}

Node::~Node() {

}

void Node::addInput(int id, NodeDataType t){
	inputs[id] = t;
}

void Node::addOutput(int id, NodeDataType t){
	outputs[id] = t;
}

void Node::removeInput(int i){
	inputs.erase(i);
}

void Node::removeOutput(int i){
	outputs.erase(i);
}


void Node::activate(ActivationType a, void * data){
}

void Node::setType(uint8_t i){
	type = i;
}

uint8_t Node::getType(){
	return type;
}

void Node::onInputChanged(int handleId){
	//inputs[uniqueId] = active;
}


void Node::render(bool editMode, float updateFactor){
	render::drawRect(getXCenter(), getYCenter(), getWidth(), getHeight(), 0);
}

void  Node::renderConnections(float updateFactor){
	int i = 0;
	int count = inputs.size();
	if(count > 0){
		float top = getYCenter() - ((count / 2) * (handleSpace));
		for(auto id : inputs){
			switch(inputs[id.first]){
			case INT:
				render::drawRect(getX(), top + (handleSpace) * i, handleSize, handleSize, 0, colorINT);
				break;
			case FLOAT:
				render::drawRect(getX(), top + (handleSpace) * i, handleSize, handleSize, 0, colorFLOAT);
				break;
			case BOOL:
				render::drawRect(getX(), top + (handleSpace) * i, handleSize, handleSize, 0, colorBOOL);
				break;
			}


			i++;
		}
	}

	i = 0;
	count = outputs.size();
	if(count > 0){
		float top = getYCenter() - ((count / 2) * (handleSpace));
		for(auto id : outputs){
			switch(outputs[id.first]){
			case INT:
				render::drawRect(getXCenter() + getWidth() / 2, top + (handleSpace) * i, handleSize, handleSize, 0, colorINT);
				break;
			case FLOAT:
				render::drawRect(getXCenter() + getWidth() / 2, top + (handleSpace) * i, handleSize, handleSize, 0, colorFLOAT);
				break;
			case BOOL:
				render::drawRect(getXCenter() + getWidth() / 2, top + (handleSpace) * i, handleSize, handleSize, 0, colorBOOL);
				break;
			}

			if(outgoingConnections.find(id.first) != outgoingConnections.end()){
				std::pair<Node *, int> p = outgoingConnections[id.first];
				//render::drawLine(getXCenter() + getWidth() / 2, top + (handleSpace) * i, p.first->getInputHandleX(p.second), p.first->getInputHandleY(p.second), 0.1, colorConnection);
				render::drawCurve(getXCenter() + getWidth() / 2, top + (handleSpace) * i, p.first->getInputHandleX(p.second), p.first->getInputHandleY(p.second), 1, 0, -1, 0, 0.1, colorConnection);
			}

			i++;
		}
	}



	//	for(Node* n : outgoing){
	//		render::drawLine(x, y, n->getXCenter(), n->getYCenter(), 0.1, colorConnection);
	//		render::drawLine(x, y, x + 0.4 * (n->getXCenter() - x), y + 0.4 * (n->getYCenter() - y), 0.1, colorDirection);
	//	}
}

int Node::getInputHandleAt(float x, float y){
	int i = 0;
	int count = inputs.size();
	if(count > 0){
		float top = getYCenter() - ((count / 2) * (handleSpace));
		for(auto id : inputs){
			if(physics::pointInRect(x, y, getX() - handleSize / 2, top + (handleSpace) * i - handleSize / 2, handleSize, handleSize)){
				return id.first;
			}
			i++;
		}
	}
	return -1;
}

int Node::getOutputHandleAt(float x, float y){
	int i = 0;
	int count = outputs.size();
	if(count > 0){
		float top = getYCenter() - ((count / 2) * (handleSpace));
		for(auto id : outputs){
			if(physics::pointInRect(x, y, getXCenter() + getWidth() / 2 - handleSize / 2, top + (handleSpace) * i - handleSize / 2, handleSize, handleSize)){
				return id.first;
			}
			i++;
		}
	}
	return -1;
}

void Node::connectOutputTo(int outputId, Node *n, int inputId){
	outgoingConnections[outputId] = std::pair<Node *, int>(n, inputId);
	n->setIncoming(inputId, this, outputId);

}

void Node::setIncoming(int handleId, Node *n, int outgoingHandle){
	incomingConnections[handleId] = std::pair<Node *, int>(n, outgoingHandle);
}

void Node::removeIncomingConnection(int handleId){
	if(incomingConnections.find(handleId) != incomingConnections.end()){
		incomingConnections[handleId].first->removeOutgoingFrom(incomingConnections[handleId].second);
		incomingConnections.erase(handleId);
	}
}

void Node::removeOutgoingConnection(int handleId){
	if(outgoingConnections.find(handleId) != outgoingConnections.end()){

		outgoingConnections[handleId].first->removeIncomingFrom(outgoingConnections[handleId].second);
		outgoingConnections.erase(handleId);
	}
}

void Node::removeIncomingFrom(int handleId){
	if(incomingConnections.find(handleId) != incomingConnections.end()){
		//TODO update value
		incomingConnections.erase(handleId);
	}

}

void Node::removeOutgoingFrom(int handleId){
	if(outgoingConnections.find(handleId) != outgoingConnections.end()){
		//TODO update value
		outgoingConnections.erase(handleId);
	}

}

float Node::getInputHandleX(int handleId){
	return getXCenter() - getWidth() / 2;
}

float Node::getInputHandleY(int handleId){
	int i = 0;
	int count = inputs.size();
	if(count > 0){
		float top = getYCenter() - ((count / 2) * (handleSpace));
		for(auto id : inputs){
			if(id.first == handleId){
				return top + (handleSpace) * i;
			}
			i++;
		}
	}

	return 0;
}

HandleData Node::getOutputHandleData(int handleId){
	if(handleData.find(handleId) != handleData.end())
		return handleData[handleId];
	return HandleData(0);
}

HandleData Node::getInputHandleData(int handleId){
	if(incomingConnections.find(handleId) != incomingConnections.end())
		return incomingConnections[handleId].first->getOutputHandleData(incomingConnections[handleId].second);
	return HandleData(0);
}

bool Node::hasHandleData(int inputHandleId){
	return incomingConnections.find(inputHandleId) != incomingConnections.end();
}

void Node::setHandleData(int id, HandleData d){
	handleData[id] = d;
	if(outgoingConnections.find(id) != outgoingConnections.end())
		outgoingConnections[id].first->onInputChanged(outgoingConnections[id].second);
}


float Node::getX(){
	return x - width / 2;
}

float Node::getY(){
	return y - height / 2;
}

float Node::getXCenter(){
	return x;
}

float Node::getYCenter(){
	return y;
}

float Node::getWidth(){
	return width;
}

float Node::getHeight(){
	return height;
}

void Node::setXCenter(float x){
	this->x = x;
	shape.x = x;
	lastX = x;
}

void Node::setYCenter(float y){
	this->y = y;
	shape.y =y;
	lastY = y;
}

void Node::setWidth(float width){
	//	if(width > 0)// TODO reenable node negative size checks
	this->width = width;
	shape.width = width;
	//	else
	//		this->width = -width;
}

void Node::setHeight(float height){
	//	if(height > 0)
	this->height = height;
	shape.height = height;
	//	else
	//		this->height = -height;
}


int Node::getIngoingCount(){
	return incomingConnections.size();
}

bool Node::doesCollide(){
	return collides;
}

bool Node::doesMove(){
	return moves;
}

void Node::setCollides(bool b){
	collides = b;

}
void Node::setMoves(bool b){
	moves = b;
}

void Node::move(float updateFactor){
	lastX = x;
	lastY = y;

	y += direction * speedY * physics::getSimulationSpeed() * updateFactor;
	x += direction * speedX * physics::getSimulationSpeed() * updateFactor;

	if(direction == 1){
		if(fabs(goalX - x) < fabs(speedX * updateFactor))
			x = goalX;
		if(fabs(goalY - y) < fabs(speedY * updateFactor))
			y = goalY;
	}
	else if(direction == -1){
		if(fabs(initX - x) < fabs(speedX * updateFactor))
			x = initX;
		if(fabs(initY - y) < fabs(speedY * updateFactor))
			y = initY;
	}


	if(x == goalX && y == goalY)
		direction = 0;
	if(x == initX && y == initY)
		direction = 0;

	shape.x = x;
	shape.y = y;
}

float Node::getMotionX(){
	return x - lastX;
}
float Node::getMotionY(){
	return y - lastY;
}

float Node::getGoalX() {
	return goalX;
}

float Node::getGoalY() {
	return goalY;
}

float Node::getInitX() {
	return initX;
}

float Node::getInitY() {
	return initY;
}

int Node::getDirection(){
	return direction;
}

void Node::setGoalX(float x){
	goalX = x;
}

void Node::setGoalY(float y){
	goalY = y;
}

void Node::setInitX(float x){
	initX = x;
}

void Node::setInitY(float y){
	initY = y;
}

void Node::setDirection(int i){
	direction = i;
}

void Node::setSpeed(float s){
	speed = s;
}

void Node::setUniqueId(int i){
	uniqueId = i;
}

void Node::recalculateMotionData(){
	speedX = speed * (fabs(goalX - initX) / (fabs(goalX - initX) + fabs(goalY - initY)));
	if(goalX - initX != 0){
		speedX *= ((goalX - initX) / fabs(goalX - initX));
	}
	speedY = speed * (fabs(goalY - initY) / (fabs(goalX - initX) + fabs(goalY - initY)));
	if(goalY - initY != 0){
		speedY *= ((goalY - initY) / fabs(goalY - initY));
	}
}

int Node::getUniqueId(){
	return uniqueId;
}

void Node::removeConnectionsTo(Node *n){
	int *handles = new int[256];
	int num = 0;
	for(auto a : incomingConnections){
		if(a.second.first == n){
			handles[num] = a.first;
			num ++;
		}
	}
	for(int i = 0; i < num; i++){
		incomingConnections.erase(handles[i]);
		onInputChanged(handles[i]);
	}

	num = 0;

	for(auto a : outgoingConnections){
		if(a.second.first == n){
			handles[num] = a.first;
			num ++;
		}
	}
	for(int i = 0; i < num; i++){
		outgoingConnections.erase(handles[i]);
	}


	delete[] handles;
}

int Node::writeIngoingConections(char *buffer, int offset){
	int o = offset;
	uint8_t id;
	buffer[offset] = incomingConnections.size(); offset ++;
	for(auto a : incomingConnections){
		buffer[offset] = a.first; offset++;
		id = a.second.first->getUniqueId();
		buffer[offset] = id << 8; offset++;
		buffer[offset] = id; offset++;
		buffer[offset] = a.second.second; offset ++;
	}
	return offset - o;

}

int Node::readIngoingConections(char *buffer, int offset, int uidOffset){
	int o = offset;
	int count = buffer[offset]; offset++; //read number of ingoing connections
	for(int i = 0; i < count; i++){
		gameLogger.log("node " + std::to_string(uniqueId) + " reading new connection for input: " + std::to_string(buffer[offset]));
		//						inputId of this node										if of other node											output id of other node
		tempIncomingConnections[buffer[offset]] = std::pair<int, int>(static_cast<uint8_t>((buffer[offset + 1] << 8) + buffer[offset + 2]) + uidOffset, buffer[offset + 3]); offset += 4;
	}

	return offset - o;

}

void Node::resolveIngoingConnections(){
	for(auto a : tempIncomingConnections){
		Node *n = NULL;
		for(int i = 0; i < world::getNodeCount(); i++){
			if(world::getNode(i)->getUniqueId() == a.second.first){
				n = world::getNode(i);
				break;
			}
		}
		if(n == NULL){
			gameLogger.log("ERROR while resolving node connection!");
			break;
		}
		gameLogger.log("resolved connection from " + std::to_string(n->getUniqueId()) + " to " + std::to_string(getUniqueId()));
		//incomingConnections[a.first] = std::pair<Node *, int>(n, a.second.second);
		n->connectOutputTo(a.second.second, this, a.first);
	}
	tempIncomingConnections.clear();
}


//void Node::resolveTempOutgoingIds(){
//	//	for(int i = 0; i < world::getNodeCount(); i++){
//	//		Node *n = world::getNode(i);
//	//		for(int h : tempOutgoingIds){
//	//			if(n->getUniqueId() == h){
//	//				addOutgoing(n);
//	//				n->onInputChanged(uniqueId, false);
//	//			}
//	//		}
//	//	}
//}

float Node::getSpeedX(){
	return speedX;
}

float Node::getSpeedY(){
	return speedY;
}

physics::Rect Node::getShape(){
	return shape;
}

bool Node::isOpaque(){
	return opaque;
}

void Node::setOpaque(bool b){
	opaque = b;
}

} /* namespace gui */

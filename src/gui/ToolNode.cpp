/*
 * ToolNode.cpp
 *
 *  Created on: Feb 9, 2015
 *      Author: dwarf
 */

#include <ToolNode.h>
#include "World.h"
#include <limits>
#include <math.h>
#include "Widget.h"
#include <iostream>
#include <string>
#include <SDL2/SDL_keycode.h>
#include "Mouse.h"
#include "Renderer.h"
#include "Warp.h"
#include "GraphicsOptions.h"
#include "CoordinateConverter.h"

#include "NodeTypeMapper.h"

#include "Node.h"
#include "NodeLever.h"
#include "NodeDoor.h"
#include "NodeAnd.h"
#include "NodeXor.h"
#include "NodeDeath.h"

namespace gui {

/**
 * returns an int[3] containing the id of the handle, the id of the selected node, and 0 for input and 1 for output
 */
int* getNodeHandle(float x, float y);
int getNodeAt(float x, float y);

std::string nodeTypeNames[] = {"lever", "door", "and", "xor", "death", "pressureplate", "value", "timer", "split", "or"};

void onTypeSelect(std::string option, void* data);

render::Color colorMoveDestination(0.7, 0.7, 0.7, 0.5);
render::Color colorConnection(0, 0, 1);

void ToolNode::setupRadialMenu(){
	RadialMenuTree root;
	//	for(std::string s : nodeTypeNames){
	//		root.add(RadialMenuTree(s));
	//	}

	RadialMenuTree input("input");
	RadialMenuTree logic("logic");
	RadialMenuTree output("output");

	input.add(RadialMenuTree("lever"));
	input.add(RadialMenuTree("pressureplate"));
	input.add(RadialMenuTree("value"));

	logic.add(RadialMenuTree("and"));
	logic.add(RadialMenuTree("or"));
	logic.add(RadialMenuTree("xor"));
	logic.add(RadialMenuTree("timer"));
	logic.add(RadialMenuTree("split"));

	output.add(RadialMenuTree("door"));
	output.add(RadialMenuTree("death"));

	root.add(input);
	root.add(logic);
	root.add(output);

	//	root.add(RadialMenuTree("lever"));
	//	root.add(RadialMenuTree("door"));
	//	root.add(RadialMenuTree("and"));
	//	root.add(RadialMenuTree("xor"));
	//	root.add(RadialMenuTree("death"));
	//	root.add(RadialMenuTree("pressureplate"));
	//	root.add(RadialMenuTree("value"));
	typeSelectMenu.setOptions(root);
	typeSelectMenu.setCallbackFunction(onTypeSelect, (void*)this);
}

ToolNode::ToolNode() {

	lastNode = -1;
	beginX = 0;
	beginY = 0;
	nodeType = 1;

	grabbedNodeGoal = -1;
	grabbedNode = -1;
	snapToX = true;
	snapToY = true;
	lastButton = 0;
	handle = -1;
	handleDirection = 0;

	setupRadialMenu();
}

ToolNode::~ToolNode() {
}

void ToolNode::render(float upateFactor){
	if(grabbedNodeGoal != -1){
		float *f = getMousePosition();
		float *v = screenToWorldSpace(f[0], f[1]);
		game::Node *n = game::world::getNode(grabbedNodeGoal);
		float x, y;
		if(snapToX)
			x = roundf(v[0] * 2) / 2;
		else
			x = v[0];
		if(snapToY)
			y = (roundf(v[1] * 2) / 2);
		else
			y = (v[1]);

		render::drawRect(x, y, n->getWidth(), n->getHeight(), 0, colorMoveDestination);

		delete[] v;
	}
	else if(grabbedNode != -1){
		float *f = getMousePosition();
		float *v = screenToWorldSpace(f[0], f[1]);
		game::Node *n = game::world::getNode(grabbedNode);
		float x, y;
		if(snapToX)
			x = roundf(v[0] * 2) / 2;
		else
			x = v[0];
		if(snapToY)
			y = (roundf(v[1] * 2) / 2);
		else
			y = (v[1]);

		render::drawRect(x, y, n->getWidth(), n->getHeight(), 0, colorMoveDestination);

		delete[] v;
	}
	else if(lastButton == 1 && lastNode != -1 && handle != -1){
		float *f = getMousePosition();
		float *v = screenToWorldSpace(f[0], f[1]);
		//render::drawLine(beginX, beginY, v[0], v[1], 0.1, colorConnection);
		render::drawCurve(beginX, beginY, v[0], v[1], 1, 0, -1, 0, 0.1,  colorConnection);

		delete[] v;
	}

	render::setRenderTarget(render::HUD);

	render::drawString(-render::options::getAspectRatio() + 0.05, 0.84, 0.05, "snap to: ");
	if(snapToX)
		render::drawString(-render::options::getAspectRatio() + 0.05 + 0.2, 0.84, 0.05, "x");
	if(snapToY)
		render::drawString(-render::options::getAspectRatio() + 0.05 + 0.23, 0.84, 0.05, "y");

	render::setRenderTarget(render::WORLD);

}

void ToolNode::begin(float x, float y, int button){
	lastButton = button;

	beginX = x;
	beginY = y;

	if(grabbedNodeGoal == -1 && grabbedNode == -1){
		if(button == 1){

			int g = getNodeAt(x, y);
			int *h = getNodeHandle(x, y);
			if(h[0] != -1){
				lastNode = h[1];
				handle = h[0];
				handleDirection = h[2];
			}
			else if(g != -1){
				grabbedNode = g;
			}
			else{
				if(snapToX)
					beginX = roundf(x);
				if(snapToY)
					beginY = roundf(y);

				game::Node *n;
				n = game::nodeTypes[nodeType]();
				//				if(nodeType == 1){
				//					n = new game::NodeLever();
				//				}
				//				else if(nodeType == 2){
				//					n = new game::NodeDoor();
				//				}
				//				else if(nodeType == 3){
				//					n = new game::NodeAnd();
				//				}
				//				else if(nodeType == 4){
				//					n = new game::NodeXor();
				//				}
				//				else if(nodeType == 5){
				//					n = new game::NodeDeath();
				//				}
				//				else if(nodeType == 6){
				//					n = new game::NodeDeath();
				//				}
				//				else{
				//					n = new game::Node();
				//				}
				n->setXCenter(x);
				n->setYCenter(y);
				lastNode = game::world::addNode(n);
			}

			delete[] h;
		}
		else if(button == 2){
			//			int index = -1;
			//
			//			game::Node *n;
			//			for(int i = 0; i < game::world::getNodeCount(); i++){
			//				n = game::world::getNode(i);
			//				if(game::physics::pointInRect(x, y, n->getX(), n->getY(), n->getWidth(), n->getHeight())){
			//					index = i;
			//					break;
			//				}
			//			}
			//			if(index != -1){
			//				lastNode = index;
			//			}
		}
		else if(button == 3){
			int index = -1;

			game::Node *n;
			for(int i = 0; i < game::world::getNodeCount(); i++){
				n = game::world::getNode(i);
				if(game::physics::pointInRect(x, y, n->getX(), n->getY(), n->getWidth(), n->getHeight()) || hypot(x - n->getXCenter(), y - n->getYCenter()) < 0.3){
					index = i;
					break;
				}
			}
			if(index != -1){
				game::world::removeNode(index);
			}
			else{

			}
		}
	}
	else if(grabbedNode == -1){
		if(button == 1){
			game::Node *n = game::world::getNode(grabbedNodeGoal);
			if(snapToX)
				n->setGoalX(roundf(x * 2) / 2);
			else
				n->setGoalX(x);
			if(snapToY)
				n->setGoalY(roundf(y * 2) / 2);
			else
				n->setGoalY(y);

			n->recalculateMotionData();
			grabbedNodeGoal = -1;
		}
		else if(button == 3){
			grabbedNodeGoal = -1;
		}
	}
	else{
		if(button == 1){
			game::Node *n = game::world::getNode(grabbedNode);
			if(snapToX)
				n->setXCenter(roundf(x * 2) / 2);
			else
				n->setXCenter(x);
			if(snapToY)
				n->setYCenter(roundf(y * 2) / 2);
			else
				n->setYCenter(y);
			grabbedNode = -1;
		}
		else if(button == 3){
			grabbedNode = -1;
		}
	}
}

void ToolNode::step(float x, float y, int button){
	if(button == 1 && lastNode != -1){
		if(handle == -1){
			game::Node *n = game::world::getNode(lastNode);
			float width, height;
			if(snapToX)
				width = roundf(x) - n->getX();
			else
				width = x - n->getX();
			if(snapToY)
				height = roundf(y) - n->getY();
			else
				height = y - n->getY();

			n->setXCenter(beginX + width / 2);
			n->setYCenter(beginY + height / 2);
			n->setWidth(width);
			n->setHeight(height);
		}
	}
}

void ToolNode::stop(float x, float y, int button){
	if(button == 2){
		//		int index = -1;
		//
		//		game::Node *n;
		//		for(int i = 0; i < game::world::getNodeCount(); i++){
		//			n = game::world::getNode(i);
		//			if(game::physics::pointInRect(x, y, n->getX(), n->getY(), n->getWidth(), n->getHeight())){
		//				index = i;
		//				break;
		//			}
		//		}
		//		if(index != -1 && lastNode != -1){
		//			if(game::world::getNode(lastNode)->hasOutgoing(game::world::getNode(index))){
		//				//game::world::getNode(lastNode)->removeOutgoingTo(game::world::getNode(index));
		//			}
		//			else{
		//				//game::world::getNode(lastNode)->addOutgoing(game::world::getNode(index));
		//			}
		//		}
	}
	else if(button == 1 && lastNode != -1){
		if(handle == -1){
			game::Node *n = game::world::getNode(lastNode);
			if(abs(n->getWidth()) < 0.3 || abs(n->getHeight()) < 0.3){
				game::world::removeNode(lastNode);
			}
			else{
				if(n->getWidth() < 0)
					n->setWidth(-n->getWidth());
				if(n->getHeight() < 0)
					n->setHeight(-n->getHeight());
				n->setInitX(n->getXCenter());
				n->setInitY(n->getYCenter());
				n->recalculateMotionData();
			}
		}
		else{
			int *h = getNodeHandle(x, y);

			if(h[1] != -1 && h[0] != -1){
				if(h[2] == 0 && handleDirection == 1 && h[1] != lastNode){
					game::world::getNode(lastNode)->connectOutputTo(handle, game::world::getNode(h[1]), h[0]);
				}
				else if(h[2] == 1 && handleDirection == 0 && h[1] != lastNode){
					game::world::getNode(h[1])->connectOutputTo(h[1], game::world::getNode(lastNode), handle);
				}
				else{
					guiLogger.log("tried to create loop");
				}
			}
			else {
				if(handleDirection == 0)
					game::world::getNode(lastNode)->removeIncomingConnection(handle);
				else if(handleDirection == 1)
					game::world::getNode(lastNode)->removeOutgoingConnection(handle);
			}

			delete[] h;

			handle = -1;
			handleDirection = 0;
			lastNode = -1;
		}
	}
	else if(button == 1 && grabbedNode != -1){
		if(hypot(beginX - x, beginY - y) < 1){
			grabbedNode = -1;
		}
		else{

		}
	}
	lastNode = -1;
}

void ToolNode::onKeyPress(int button, int modifier, bool pressed, Widget *parent){
	if(button == SDLK_t){
		if(pressed){
			if(!parent->hasChild(&typeSelectMenu)){
				float *f = getMousePosition();
				typeSelectMenu.setLocation(f[0], f[1]);
				parent->addWidget(&typeSelectMenu);
			}
		}
		else{
			onTypeSelect(typeSelectMenu.getSelectedOption(), this);
			parent->removeWidget(&typeSelectMenu);
		}
	}
	else if(button == SDLK_g){
		float *f = getMousePosition();
		float *v = screenToWorldSpace(f[0], f[1]);
		int index = -1;

		game::Node *n;
		for(int i = 0; i < game::world::getNodeCount(); i++){
			n = game::world::getNode(i);
			if(game::physics::pointInRect(v[0], v[1], n->getX(), n->getY(), n->getWidth(), n->getHeight())){
				index = i;
				break;
			}
		}

		if(index != -1){
			grabbedNodeGoal = index;
		}

		delete[] v;

	}
	else if(button == SDLK_x && !pressed){
		snapToX = !snapToX;
	}
	else if(button == SDLK_y && !pressed){
		snapToY = !snapToY;
	}
}

std::string ToolNode::getName(){
	return "Node - " + nodeTypeNames[nodeType - 1];
}

void ToolNode::setNodeType(int i){
	nodeType = i;
}

int* getNodeHandle(float x, float y){
	int* h = new int[3];
	h[0] = -1;
	for(int i = 0; i < game::world::getNodeCount(); i++){
		h[0] = game::world::getNode(i)->getInputHandleAt(x, y);
		if(h[0] != -1){
			h[1] = i;
			h[2] = 0;
			return h;
		}
		h[0] = game::world::getNode(i)->getOutputHandleAt(x, y);
		if(h[0] != -1){
			h[1] = i;
			h[2] = 1;
			return h;
		}
	}

	return h;
}

int getNodeAt(float x, float y){
	for(int i = 0; i < game::world::getNodeCount(); i++){
		game::Node *n = game::world::getNode(i);
		if(game::physics::pointInRect(x, y, n->getX(), n->getY(), n->getWidth(), n->getHeight())){
			return i;
		}
	}
	return -1;
}

void onTypeSelect(std::string option, void* data){
	ToolNode *n = (ToolNode*) data;


	if(option.find("-") > 0){
		option = option.substr(option.find("-") + 1, option.size());

		for(unsigned int i = 0; i < sizeof(nodeTypeNames) / sizeof(nodeTypeNames[0]); i++){
			if(nodeTypeNames[i] == option){
				n->setNodeType(i + 1);
				return;
			}

		}
	}

	//	if(option == "lever"){
	//		n->setNodeType(1);
	//	}
	//	else if(option == "door"){
	//		n->setNodeType(2);
	//	}
	//	else if(option == "and"){
	//		n->setNodeType(3);
	//	}
	//	else if(option == "xor"){
	//		n->setNodeType(4);
	//	}
	//	else if(option == "death"){
	//		n->setNodeType(5);
	//	}
	//	else if(option == "pressureplate"){
	//		n->setNodeType(5);
	//	}
}


} /* namespace gui */

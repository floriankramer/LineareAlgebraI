#include "NodeResource.h"
#include "warp/Renderer.h"
#include "TaskGather.h"
#include "WorldClient.h"
#include "Server.h"
#include <climits>
#include "Options.h"
#include "Task.h"

namespace ant{

	warp::Color colorNodeResource(0, 1, 1, 1);
	warp::Color colorBlack(0, 0, 0, 1);
	warp::Color colorGray(0.4, 0.4, 0.4, 1);

	NodeResource::NodeResource(int x, int y, int resourceType, bool onServer) : Node(x, y, onServer), resourceType(resourceType), cooldown(options::getResourceRegrowthCooldown()), freeResources(0), exhaustionCooldown(0), connected(false), hasFreeTask(false) {

		setInteresting(true);
		setDeletable(false);
		setResourceMax(resourceType, options::getResourceMaxStored());
		storeResource(getResourceType(), options::getResourceStartStored());

	}

	NodeResource::NodeResource(int x, int y, int resourceType, unsigned int id, bool onServer) : Node(x, y, id, onServer), resourceType(resourceType), cooldown(1000), freeResources(0), exhaustionCooldown(0), connected(false), hasFreeTask(false) {
		setInteresting(true);
		setDeletable(false);
		setResourceMax(resourceType, options::getResourceMaxStored());
		storeResource(getResourceType(), options::getResourceStartStored());
	}

	NodeResource::~NodeResource() {

	}


	void NodeResource::updateTaskState() {
			if(freeResources > 0 && !hasFreeTask && getDistanceTo(world::getMainHive()) != INT_MAX) {
				TaskGather *nt = new TaskGather(this);
				nt->setStart(this);
				nt->setGoal(world::getMainHive());
				world::addTask(nt);	
				hasFreeTask = true;
			}
	}

	void NodeResource::render() {
		warp::drawCircle(getX(), getY(), 70 * getPercentageFull(resourceType) + 27, resourceColors[resourceType]);	
//		if(hasFreeTask) {
//			warp::drawString(getX() + 70, getY(), 70, "Offering Task");
//		}
		Node::render();
		if(exhaustionCooldown > 0) {
			warp::drawCircle(getX(), getY(), 20, colorGray);
		}
		if(options::isDebugRendering()) {
			warp::drawCircle(getX(), getY(), 70 * (freeResources / 30.0) + 27, hasFreeTask ? colorGray : colorBlack);
		}
	}

	void NodeResource::update(float updateFactor) {	
		if(onServer) {
			if(exhaustionCooldown > 0) {
				exhaustionCooldown -= updateFactor * 1000;
			} else if(cooldown > 0) {
				cooldown -= updateFactor * 1000;
			} else if(getSpaceLeft(resourceType) > 0) {	
				storeResource(resourceType, options::getResourceRegrowthAmount());
				unsigned char d[5];
				writeIntToBuffer(getId(), d, 0);
				d[4] = options::getResourceRegrowthAmount();
				server::broadcastPacket(NODE_RESOURCE_REGROWTH, d, 5);	
				cooldown = options::getResourceRegrowthCooldown();
			}
		}
		Node::update(updateFactor);
	}

	int NodeResource::getResources(int resource, unsigned int amount) {
		int i = ResourceStorage::getResources(resource, amount);
		if(onServer && getAmountStored(getResourceType()) < options::getResourceExhaustionThreshold()) {
			exhaustionCooldown = options::getResourceExhaustionCooldown();
		}
		return i;
	}

	int NodeResource::storeResource(int resource, unsigned int amount) {
		int i = ResourceStorage::storeResource(resource, amount);
		if(!onServer) {
			freeResources += amount - i;
			updateTaskState();
		}
		return i;
	}


	int NodeResource::getResourceType() {
		return resourceType;
	}


	void NodeResource::updatePathTo(Node *goal, int dist) {
		if(goal == world::getMainHive()) {
			updateTaskState();
		}
		Node::updatePathTo(goal, dist);
	}


	void NodeResource::connectTo(Node *n) {
		Node::connectTo(n);
		if(!connected && getDirectionTo(world::getMainHive()) != NULL) {
			connected = true;
		}
		else if(connected && getDirectionTo(world::getMainHive()) == NULL) {
			connected = false;
		}
	}




	void NodeResource::onTaskAccepted(Task *t) {
		if(t->getType() == ECONOMY) {
			hasFreeTask = false;
			freeResources --; //TODO remove number of resources the ant will actually harvest (in case it can be more than one)
			updateTaskState();
		}
		Node::onTaskAccepted(t);
	}


	void NodeResource::onTaskAborted(Task *t) {
		if(t->getType() == ECONOMY) {

			if(t->getState() == 0) {
				hasFreeTask = true;
			}
			freeResources ++;
		}

		Node::onTaskAborted(t);
	}


	void NodeResource::onTaskFinished(Task *t) {
		Node::onTaskFinished(t);
	}

	
	int NodeResource::getCreationPacketSize() {
		return 17;
	}

	void NodeResource::writeCreationPacket(unsigned char *buff) {
		writeIntToBuffer(getX(), buff, 0);
		writeIntToBuffer(getY(), buff, 4);
		writeIntToBuffer(getId(), buff, 8);
		buff[12] = getResourceType();
		writeIntToBuffer(getAmountStored(getResourceType()), buff, 13);
	}

	PACKET_TYPE NodeResource::getCreationPacketType() {
		return NEW_NODE_RESSOURCE;
	}

	NodeResource *createNodeResourceFromPacket(unsigned char *buff) {
		NodeResource *nr = new NodeResource(readIntFromBuffer(buff, 0), readIntFromBuffer(buff, 4), buff[12], readIntFromBuffer(buff, 8), false);
		nr->storeResource(nr->getResourceType(), readIntFromBuffer(buff, 13));
		return 	nr;
	}

  bool NodeResource::canBuildPathFrom() {
    return false;
  }

}

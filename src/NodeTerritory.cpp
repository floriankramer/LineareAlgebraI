#include "NodeTerritory.h"
#include "warp/Renderer.h"
#include "WorldClient.h"
#include "TaskConquer.h"
#include "Client.h"
#include "Server.h"
#include "Options.h"

namespace ant{


	warp::Color playerColorNone(0.3, 0.3, 0.3, 1);
	warp::Color playerColors[] = {	
		warp::Color(0, 1, 0, 1), // green
		warp::Color(1, 1, 0, 1), // yellow
		warp::Color(1, .3, 0, 1), // orange
		warp::Color(0, 1, 0.94, 1), // turqoise
		warp::Color(0.47, 0, 1, 1), // purple
		warp::Color(0.399, 0.214, 0.015, 1), // brown
		warp::Color(1, 0, 1, 1)  // pink

	};

	NodeTerritory::NodeTerritory(int x, int y, bool onServer) : Node(x, y, onServer), owner(-1), freeJobs(0){
		setInteresting(true);
		setDeletable(false);

	}


	NodeTerritory::NodeTerritory(int x, int y, unsigned int id, bool onServer) : Node(x, y, id, onServer), owner(-1), freeJobs(0){
		setInteresting(true);
		setDeletable(false);
	}



	NodeTerritory::~NodeTerritory(){

	}


	void NodeTerritory::render(){
		Node::render();
		warp::drawCircle(getX(), getY(), 20, owner == -1 ? playerColorNone : playerColors[owner]);

	}


	void NodeTerritory::update(float updateFactor){
		Node::update(updateFactor);
	}



	void NodeTerritory::updatePathTo(Node *goal, int dist){
		Node::updatePathTo(goal, dist);
		if(goal == world::getMainHive()){
			updateTaskState();
		}
	}


	void NodeTerritory::updateTaskState(){
		if(freeJobs < 1 && owner != world::getPlayerId() && getDirectionTo(world::getMainHive()) != NULL){
			TaskConquer *t = new TaskConquer(this);
			t->setStart(this);
			world::addTask(t);
		}
	}

	void NodeTerritory::updateConqueringProgressBy(int player, int amount){
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
				server::broadcastPacket(NODE_TERRITORY_CONQUERED, data, 8);
			}
		}
	}


	void NodeTerritory::onTaskAccepted(Task *t){
		if(t->getType() == CONQUER){
			freeJobs --;
			updateTaskState();
		}

		Node::onTaskAccepted(t);
	}


	void NodeTerritory::onTaskAborted(Task *t){
		if(t->getType() == CONQUER && t->getState() == 0){
			freeJobs ++;
		}
		Node::onTaskAborted(t);
	}

	void NodeTerritory::setOwner(int i){
		owner = i;
	}

	int NodeTerritory::getOwner(){
		return owner;
	}


	void NodeTerritory::onTaskFinished(Task *t){
		if(t->getType() == CONQUER){
			unsigned char data[9];
			writeIntToBuffer(getId(), data, 0);
			writeIntToBuffer(world::getPlayerId(), data, 4);
			data[8] = t->getServant()->getConqueringPower(); //TODO use ant conquer strength
			client::sendPacket(NODE_TERRITORY_CONQUERING, data, 9);
		}
		Node::onTaskFinished(t);
	}


	int NodeTerritory::getCreationPacketSize(){
		return 12;
	}


	void NodeTerritory::writeCreationPacket(unsigned char *buff){
		writeIntToBuffer(getId(), buff, 0);
		writeIntToBuffer(getX(), buff, 4);
		writeIntToBuffer(getY(), buff, 8);
	}


	PACKET_TYPE NodeTerritory::getCreationPacketType(){
		return NEW_NODE_TERRITORY;
	}


	NodeTerritory *createNodeTerritoryFromPacket(unsigned char *buff){
		return new NodeTerritory(readIntFromBuffer(buff, 4), readIntFromBuffer(buff, 8), readIntFromBuffer(buff, 0), false);
	}


}

#include "NodeTunnelHead.h"
#include "warp/Renderer.h"
#include "TaskDig.h"
#include <cmath>
#include "WorldClient.h"
#include <algorithm>
#include "Client.h"
#include "Options.h"

namespace ant{

	warp::Color nodeTunnelColor(0.5, 0.4, 1, 1);

	NodeTunnelHead::NodeTunnelHead(int x, int y, int goalX, int goalY, int endX, int endY) :
    Node(x, y, false),
    startX(x),
    startY(y),
    goalX(goalX),
    goalY(goalY),
    endX(endX),
    endY(endY),
    hasFreeTask(false),
    numWorkers(0), 
    digTasks() {
      setInteresting(true);
      workToBeDone = std::hypot(goalX - x, goalY - y) * options::getTunnelCostPerUnit() + options::getTunnelBaseCost();
      totalWork = workToBeDone;
      updateTaskState();
      dist = workToBeDone;
    }


  NodeTunnelHead:: ~NodeTunnelHead(){

  }


	void NodeTunnelHead::render(){
		warp::drawCircle(getX(), getY(), 15 + 65 * (dist - workToBeDone + 0.0) / dist, nodeTunnelColor);	
		Node::render();
	}


	void NodeTunnelHead::update(float updateFactor){

	}

	void NodeTunnelHead::updateTaskState(){
		if(!hasFreeTask && numWorkers < 20){
			TaskDig *t = new TaskDig(this);
			t->setStart(this);	
			world::addTask(t);
			digTasks.push_back(t);
			hasFreeTask = true;
		}
	}



	void NodeTunnelHead::onTaskAccepted(Task *t){
		numWorkers ++;
		hasFreeTask = false;
		updateTaskState();
	}


	void NodeTunnelHead::onTaskAborted(Task *t){
		numWorkers --;
		hasFreeTask = true;
	}


	void NodeTunnelHead::onTaskFinished(Task *t){
		numWorkers --;
		workToBeDone -= 10;
    setX(startX + (goalX - startX) * (1 - ((workToBeDone + 0.0) / totalWork))); 
    setY(startY + (goalY - startY) * (1 - ((workToBeDone + 0.0) / totalWork))); 
    // TODO find out why this doesn't do anything 
    // TODO actually dig tunnel
    
		digTasks.erase(std::remove(digTasks.begin(), digTasks.end(), t), digTasks.end());
		updateTaskState();
		
		printf("still got %d to do\n", workToBeDone);
		if(workToBeDone < 0){
 			Node *n = world::createNodeAt(endX, endY);
			if(getNeighborCount() > 0){
				n->connectTo(getNeighbor(0));
			}

			for(TaskDig *d : digTasks){
				world::deleteTask(d);
			}
			digTasks.clear();

			unsigned char data[17];
		
			writeIntToBuffer(getX(), data, 0);
			writeIntToBuffer(getY(), data, 4);
			writeIntToBuffer(goalX, data, 8);
			writeIntToBuffer(goalY, data, 12);
			data[16] = 50;

			client::sendPacket(NEW_TUNNEL, data, 17);


			world::deleteNodeAt(getX(), getY()); //TODO move out of here
		}
	}
}

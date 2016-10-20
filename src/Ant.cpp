#include "Ant.h"
#include "WorldClient.h"
#include <cmath>
#include <cstdio>
#include "Node.h"
#include "Task.h"
#include "Options.h"


namespace ant{
	
	int idleAnts = 0;
	unsigned int antCounter = 0;


	Ant::Ant( int x,  int y,  int id) : NetAnt(x, y, 1, id, world::getPlayerId()), task(NULL), goal(NULL), timeToGoal(0), sleepTime(0), currentResourceType(-1), lifetime(120000), harvestTimeReduction(0), idle(false), conqueringPower(options::getAntConqueringPower()), rallySpeedBonus(0), speed(200){
		setSpeedX(0);
		setSpeedY(0);
		setResourceMaxAll(1);
		setResourceMaxTotal(1);
		NetAnt::update(0); //hack for correct inital position sync
		sendCreate();
		for(int i = 0; i < NUM_TASK_TYPES; i++){
			abilities[i] = true;
		}
				antCounter ++;
	}

	Ant::~Ant(){	
		antCounter --;
		if(task != NULL){
			delete task;
			task = NULL;
		}
	}

	void Ant::render(){
		if(currentResourceType == -1){
			NetAnt::render();
		}
		else{
			warp::drawCircle(getVisualX(), getVisualY(), 8, resourceColors[currentResourceType]);
		}
	}

  void Ant::renderVision() {
      warp::drawCircle(getVisualX(), getVisualY(), 50, warp::Color(1, 1, 1, 0));
  }

	int Ant::getResources(int resource, unsigned int amount){
		currentResourceType = -1;
		return ResourceStorage::getResources(resource, amount);
	}

	int Ant::storeResource(int resource, unsigned int amount){
		currentResourceType = resource;
		return ResourceStorage::storeResource(resource, amount);
	}


	void Ant::taskDeleted(){
		task = NULL;
	}

	void Ant::setCurrentNode(Node *n){
		if(current != NULL){
			current->removeComingFrom(this);
		}
		current = n;
		current->addComingFrom(this);
	}

	void Ant::setLifetime(int i){
		lifetime = i;
	}

	void Ant::setRallySpeedBonus(int i){
		rallySpeedBonus = i;
	}

	bool Ant::canDoTask(TaskType t){
		return abilities[t];
	}

	bool Ant::canDoTask(int i){
		return abilities[i];
	}

	void Ant::setAbility(TaskType t, bool b){
		abilities[t] =  b;
	}

	void Ant::setSpeed(int i){
		speed = i;
	}
	
	void Ant::setHarvestTimeReduction(int i){
		harvestTimeReduction = i;
	}


	void Ant::update(float updateFactor){
		if(sleepTime > 0){
			sleepTime -= updateFactor * 1000;
		}
		else{
			if(task == NULL){

				task = world::getTask(this);
				if(task == NULL){
					sleepTime = 1000;
					if(!idle){
						idleAnts ++;
						idle = true;
					}
				}
				else{
					task->begin(this);
					if(task->isBusywork() && !idle){
						idle = true;
						idleAnts ++;
					}
					else if(!task->isBusywork() && idle){
						idle = false;
						idleAnts --;
					}
				}
				//if(task != NULL){
				//	printf("got task\n");
				//}
			}
			if(goal == NULL && task != NULL && current != NULL){
				if(current == task->getNext()){
					task->advance(this);
					if(task->isFinished()){
						delete task;
						task = NULL;
						setSpeedX(0);
						setSpeedY(0);
						sendSync();
					}
				}
				if(task != NULL){
					goal = current->getDirectionTo(task->getNext());
					if(goal != NULL){
						recomputeSpeed();
						goal->addGoingTo(this);
					}
					else{
						if(task->abort()){
							delete task;
						}
						task = NULL;
						setSpeedX(0);
						setSpeedY(0);
						sendSync();
						//TODO no connection to task. abort.
					}
				}
			}
			NetAnt::update(updateFactor);
			timeToGoal -= updateFactor * 1000;

			if(goal != NULL && (timeToGoal <= 0 || hypot(goal->getX() - getX(),  goal->getY() - getY()) < 5)){
				setBaseX(goal->getX());
				setBaseY(goal->getY());
				setOffsetX(0);
				setOffsetY(0);
				setSpeedX(0);
				setSpeedY(0);
				if(current != NULL){
					current->removeComingFrom(this);
				}
				goal->removeGoingTo(this);
				goal->addComingFrom(this);
				current = goal;
				goal = NULL;
				if(current == task->getNext()){	
					sleepTime = task->getWorkTime();
				       	if(task->getType() == ECONOMY){
						sleepTime -= harvestTimeReduction;
					}	
					if(sleepTime > 0){
						sendSync();
            recomputeMotionNoise();
					}

				}
			}

			lifetime -= updateFactor * 1000;
			if(lifetime < 0){
				die();	
			}
		}

	}

	void Ant::onDeleteCurrent(){
		//	printf("on delete current\n");
		if(current == goal){
			// should never happen, as ant should only be registered whith the goal
			printf("ERROR: ant registered as 'going from' while current == goal\n");	
		}
		else if(goal == NULL){
			die();
			//killAnt(this);
		}
		else{
			current->removeComingFrom(this);
			current = goal;	
		}
	}

	void Ant::onDeleteGoal(){
		//	printf("on delete goal\n");
		if(current == goal){
			//		printf("killing ant\n");
			//current = goal = NULL;
			//killAnt(this);
			die();
		}
		else{
			//		printf("turning ant around\n");
			goal = current;
			goal->addGoingTo(this);
			current->removeComingFrom(this);
			recomputeSpeed();
		}
	}

	void Ant::recomputeSpeed(){
		int dx = 0, dy = 0;
		float speedMultiplier = 1;
		if(goal != current){
			speedMultiplier = current->getSpeedMultiplierTo(goal);
			//		printf("normal movement\n");
			dx = goal->getX() - current->getX();
			dy = goal->getY() - current->getY();
		}
		else{
			//		printf("return movement\n");
			dx = goal->getX() - getX();
			dy = goal->getY() - getY();	

		}
		double l = hypot(dx, dy);
		//	printf("distance: %f\n", l);
		if(dx == 0 && dy == 0){
			l = 1;
		}
		float s = speed;
		if(task != NULL && task->getType() == MILITARY){
			s += rallySpeedBonus;
		}
		s *= speedMultiplier;  // time without heart beat
		timeToGoal = l / (s * 0.5) * 1000;
		setSpeedX(dx / l * s);
		setSpeedY(dy / l * s);
		sendSync();
    recomputeMotionNoise();
	}

	void Ant::onDeath(){
		if(task != NULL){
			if(task->abort()){
				delete task;
			}
			task = NULL;
		}
		if(goal != NULL){
			goal->removeGoingTo(this);
		}
		if(current != NULL){
			current->removeComingFrom(this);
		}
		sendDeath();
	}


	int Ant::getConqueringPower(){
		return conqueringPower;
	}

	void Ant::setConqueringPower(int i){
		conqueringPower = i;
	}

	Node *Ant::getCurrentNode(){
		return current;
	}


	int getNumIdleAnts(){
		return idleAnts;
	}

	unsigned int getAntCounter(){
		return antCounter;
	}
}

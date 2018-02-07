#include "Task.h"
#include "WorldClient.h"
#include <climits>
#include "Node.h"
#include "Task.h"

namespace ant{

	Task::Task(TaskProvider *t, TaskType type) : start(NULL), goal(NULL), state(0), provider(t), walkingDistance(INT_MAX), servant(NULL), type(type){

	}

	Task::~Task(){
		if(servant != NULL){
			servant->taskDeleted();
		}
	}


	bool Task::operator<(const Task& t){
		return (this->walkingDistance < t.walkingDistance);
	}	

	TaskType Task::getType(){
		return type;
	}

	bool Task::test(Ant *a){
		if(start != NULL && goal != NULL){
			return a->getCurrentNode()->getDirectionTo(start) != NULL && start->getDirectionTo(goal) != NULL;
		}	
		return false;
	}

	void Task::setStart(Node *n){
		start = n;
	}

	void Task::setGoal(Node *n){
		goal = n;
	}

	void Task::begin(Ant *a){
		if(provider != NULL){
			provider->onTaskAccepted(this);	
		}
		servant = a;
	}

	Node *Task::getGoal(){
		return goal;
	}

	Node *Task::getStart(){
		return start;
	}


	Node *Task::getNext(){
		return state == 0 ? start : goal;
	}


	void Task::advance(Ant *a){
		state ++;
		if(isFinished() && provider != NULL){
			provider->onTaskFinished(this);
		}
	}

	Ant *Task::getServant(){
		return servant;
	}

	bool Task::isFinished(){	
		return state > 1;
	}

	unsigned int Task::getState(){
		return state;
	}

	bool Task::isBusywork(){
		return false;
	}

	bool Task::abort(){
		if(provider != NULL){
			provider->onTaskAborted(this);
		}
		if(state == 0){
			world::addTask(this);
			return false;
		}
		return true; //delete task
	}


	int Task::getWalkingDistance(){
		return walkingDistance;
	}

	void Task::updateWalkingDistance(Ant *a){
		if(goal != NULL && start != NULL){
			walkingDistance = a->getCurrentNode()->getDistanceTo(start) + start->getDistanceTo(goal);
		}
		else{
			walkingDistance = INT_MAX;
		}
	}

	int Task::getWorkTime(){
		return 0;
	}

}

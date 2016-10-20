#include "TaskDig.h"
#include "Node.h"
#include "Ant.h"

namespace ant{

	TaskDig::TaskDig(TaskProvider *t) : Task(t, CONSTRUCTION){	
	}

	TaskDig::~TaskDig(){

	}

	bool TaskDig::test(Ant *a){
		return getStart() != NULL && a->getCurrentNode()->getDirectionTo(getStart()) != NULL; 
	}

	void TaskDig::advance(Ant *a){
		Task::advance(a);
	}

	bool TaskDig::isFinished(){	
		return getState() > 0;
	}

	int TaskDig::getWorkTime(){
		if(getState() == 0){
			return 1000;
		}
		return 0;
	}
}

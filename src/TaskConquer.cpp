#include "TaskConquer.h"
#include "Node.h"
#include "Ant.h"
#include "Options.h"

namespace ant{

	TaskConquer::TaskConquer(TaskProvider *t) : Task(t, CONQUER){	
	}

	TaskConquer::~TaskConquer(){

	}

	bool TaskConquer::test(Ant *a){
		return getStart() != NULL && a->getCurrentNode()->getDirectionTo(getStart()) != NULL; 
	}

	void TaskConquer::advance(Ant *a){
		Task::advance(a);
	}

	bool TaskConquer::isFinished(){	
		return getState() > 0;
	}

	int TaskConquer::getWorkTime(){
		if(getState() == 0){
			return options::getConqueringTime();
		}
		return 0;
	}
}

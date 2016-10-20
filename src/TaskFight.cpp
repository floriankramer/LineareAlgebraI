#include "TaskFight.h"
#include "Ant.h"
#include "Node.h"


namespace ant{

	TaskFight::TaskFight(TaskProvider *t) : Task(t, MILITARY){

	}

	TaskFight::~TaskFight(){

	}

	bool TaskFight::test(Ant *a){
		if(a->getCurrentNode() != NULL && getStart() != NULL){	
			if(a->getCurrentNode()->getNeighborCount() > 0){		
				return a->getCurrentNode()->getDirectionTo(getStart()) != NULL;
			}
		}
		return false;
	}	



	int TaskFight::getWorkTime(){
		if(getState() == 0){
			return 1000;
		}
		return 0;
	}


	bool TaskFight::isFinished(){
		return getState() > 0;
	}
}

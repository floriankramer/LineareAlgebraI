#include "TaskWander.h"
#include <cstdio>
#include "Node.h"
#include "Ant.h"

namespace ant{

	TaskWander::TaskWander() : Task(NULL, ECONOMY){

	}

	TaskWander::~TaskWander(){

	}

	bool TaskWander::test(Ant *a){

		if(a->getCurrentNode() != NULL){	
			if(a->getCurrentNode()->getNeighborCount() > 0){	
				setStart(a->getCurrentNode()->getNeighbor(std::rand() % a->getCurrentNode()->getNeighborCount()));	
				return true;
			}
		}
		return false;
	}

	bool TaskWander::isFinished(){
		return getState() > 0;
	}

	bool TaskWander::isBusywork(){
		return true;
	}

}

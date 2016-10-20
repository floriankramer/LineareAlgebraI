#include "TaskGather.h"
#include "NodeResource.h"
#include "Client.h"
#include "Options.h"


namespace ant{

	TaskGather::TaskGather(TaskProvider *t) : Task(t, ECONOMY){

	}

	TaskGather::~TaskGather(){

	}

	bool TaskGather::test(Ant *a){
		return Task::test(a);
	}

	void TaskGather::advance(Ant *a){
		
		if(getState() == 0){
			NodeResource *r = dynamic_cast<NodeResource*>(getStart());
			int amount = r->getResources(r->getResourceType(), 1);
			a->storeResource(r->getResourceType(), amount); //TODO check if ant can carry resource??
			if(amount > 0){
				unsigned char d[5];
				writeIntToBuffer(r->getId(), d, 0);
				d[4] = amount;
				client::sendPacket(NODE_RESOURCE_HARVEST, d, 5);
			}
			else{
				Task::advance(a); //ensure task is finished, as we were unable to complete it TODO think about aborting here
			}

		}
		else if(getState() == 1){
			NodeResource *r = dynamic_cast<NodeResource*>(getStart());
			ResourceStorage *h = dynamic_cast<ResourceStorage *>(getGoal());
			h->storeResource(r->getResourceType(), a->getResources(r->getResourceType(), 1));

		}
		Task::advance(a);
	
	}

	int TaskGather::getWorkTime(){
		if(getState() == 0){
			return options::getResourceHarvestTime();
		}
		return 0;
	}
}

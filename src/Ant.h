#ifndef ANT_H_
#define ANT_H_

#include "NetAnt.h"
#include "ResourceStorage.h"
#include "TaskTypes.h"

namespace ant{

	class Task;
	class Node;

	int getNumIdleAnts();

	class Ant : public NetAnt, public ResourceStorage{
		public:
			Ant(int x, int y, int id);
			virtual ~Ant();
			virtual void update(float updateFactor);	
			Node *getCurrentNode();
			void onDeleteCurrent();
			void onDeleteGoal();
			void onDeath();
			virtual void render();
      virtual void renderVision();
			void taskDeleted();
			void setCurrentNode(Node *n);
			void setLifetime(int i);
			void setSpeed(int i);
			void setHarvestTimeReduction(int i);

			int getConqueringPower();
			void setConqueringPower(int i);

			void setRallySpeedBonus(int i);
			bool canDoTask(TaskType t);
			bool canDoTask(int i);
			void setAbility(TaskType t, bool b);

			virtual int getResources(int resource, unsigned int amount);	
			virtual int storeResource(int resource, unsigned int amount);
		private:
			void recomputeSpeed();
			Task *task;
			Node *goal;	
			unsigned int timeToGoal, sleepTime; 
			int currentResourceType;
			int lifetime;
			int harvestTimeReduction;	
			bool idle;
			int conqueringPower;
			bool abilities[NUM_TASK_TYPES]; 
			int rallySpeedBonus;
			int speed;


	};

	unsigned int getAntCounter();
}

#endif

#ifndef TASK_H_
#define TASK_H_

#include "TaskTypes.h"

namespace ant{

	class Node;
	class Ant;
	class Task;


	class TaskProvider{
		public:
			virtual void onTaskAccepted(Task *t){}
			virtual void onTaskAborted(Task *t){}
			virtual void onTaskFinished(Task *t){}
	};

	class Task{
		public:
			Task(TaskProvider *t, TaskType type);
			virtual ~Task();
			virtual bool test(Ant *a);
			void begin(Ant *a);
			void setStart(Node *n);
			void setGoal(Node *n);
			Node *getStart();
			Node *getGoal();
			Node *getNext();
			virtual int getWorkTime();
			virtual void advance(Ant *a);
			virtual bool isFinished();
			virtual bool abort();
			int getWalkingDistance();
			void updateWalkingDistance(Ant *a);
			unsigned int getState();
			virtual bool isBusywork();
			bool operator < (const Task& t); 
			TaskType getType();
			Ant *getServant();
		private:
			Node *start, *goal;
			unsigned int state;
			TaskProvider *provider;
			int walkingDistance;
			Ant *servant;
			TaskType type;

	};

}

#endif

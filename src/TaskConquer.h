#ifndef ANT_TASK_CONQUER_H
#define ANT_TASK_CONQUER_H

#include "Task.h"

namespace ant{

	class TaskConquer : public Task{

		public:
			TaskConquer(TaskProvider *t);
			virtual ~TaskConquer();
			virtual bool test(Ant *a);
			virtual void advance(Ant *a);	
			virtual int getWorkTime();
			virtual bool isFinished();	
			

	};

}


#endif


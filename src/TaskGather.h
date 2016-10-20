#ifndef TASK_GATHER_H_
#define TASK_GATHER_H_

#include "Task.h"
#include "Ant.h"

namespace ant{

	class TaskGather: public Task{
		public:
			TaskGather(TaskProvider *t);
			virtual ~TaskGather();
			virtual bool test(Ant *a);
			virtual void advance(Ant *a);	
			virtual int getWorkTime();

	};
}
#endif

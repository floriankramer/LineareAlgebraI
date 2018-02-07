#ifndef TASK_WANDER_H_
#define TASK_WANDER_H_

#include "Task.h"

namespace ant{

	class TaskFight : public Task{
		public:
			TaskFight(TaskProvider *t);
			virtual ~TaskFight();
			virtual bool test(Ant *a);
			virtual bool isFinished();		
			virtual int getWorkTime();

	};

}
#endif

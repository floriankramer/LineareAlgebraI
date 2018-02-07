#ifndef TASK_WANDER_H_
#define TASK_WANDER_H_

#include "Task.h"

namespace ant{

	class TaskWander : public Task{
		public:
			TaskWander();
			virtual ~TaskWander();
			virtual bool test(Ant *a);
			virtual bool isFinished();
			virtual bool isBusywork();


	};

}
#endif

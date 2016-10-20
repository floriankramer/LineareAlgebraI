#ifndef TASKDIG_H_
#define TASKDIG_H_

#include "Task.h"

namespace ant{

	class TaskDig : public Task{
		public:
			TaskDig(TaskProvider *t);
			virtual ~TaskDig();
			virtual bool test(Ant *a);
			virtual void advance(Ant *a);	
			virtual int getWorkTime();
			virtual bool isFinished();


	};

}
#endif

#ifndef NODETUNNELHEAD_H_
#define NODETUNNELHEAD_H_

#include "Node.h"
#include "TaskDig.h"


namespace ant{

	class NodeTunnelHead : public Node{

		public:
			NodeTunnelHead(int x, int y, int goalX, int goalY, int endX, int endY);
			virtual ~NodeTunnelHead();
			virtual void render();
			virtual void update(float updateFactor);


			virtual void onTaskAccepted(Task *t);
			virtual void onTaskAborted(Task *t);
			virtual void onTaskFinished(Task *t);

		private:
			void updateTaskState();
      int startX, startY;
			int goalX, goalY;
			int endX, endY;
			bool hasFreeTask; 
			int numWorkers;
			int workToBeDone;
      int totalWork;
			int dist;
			std::vector<TaskDig*> digTasks;
	};

}

#endif

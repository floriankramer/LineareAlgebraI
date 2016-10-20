#ifndef NODE_HIVE_H
#define NODE_HIVE_H

#include "Node.h"
#include "ResourceStorage.h"

namespace ant{


	class NodeHive : public Node, public ResourceStorage{

		public:
			NodeHive(int x, int y, int owner, bool onServer);
			NodeHive(int x, int y, int id, int owner, bool onServer);
			virtual ~NodeHive();
			virtual void render();
			virtual void update(float updateFactor);
			virtual int getCreationPacketSize();
			virtual void writeCreationPacket(unsigned char *buff);
			virtual PACKET_TYPE getCreationPacketType();

			virtual void updatePathTo(Node *goal, int dist);


			virtual void onTaskAccepted(Task *t);
			virtual void onTaskAborted(Task *t);
			virtual void onTaskFinished(Task *t);

			void updateConqueringProgressBy(int player, int amount);

			void setOwner(int i);	
      float getPulseStrength();

		private:
			void produceAnt();
			int spawnCounter;	

			void updateTaskState();

			int owner;
			int freeJobs, numWorkers;
			std::map<int, int> conqueringProcess;
      int pulseTimer;
      float pulseValue, pulseRenderValue;

	};


	NodeHive *createNodeHiveFromPacket(unsigned char *buff);

}

#endif

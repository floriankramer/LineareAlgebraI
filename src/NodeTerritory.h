#ifndef ANT_NODE_TERITORRY_H
#define ANT_NODE_TERITORRY_H

#include "Node.h"
#include <map>

namespace ant{

	class NodeTerritory : public Node{
		public:
			NodeTerritory(int x, int y, bool onServer);
			NodeTerritory(int x, int y, unsigned int id, bool onServer);

			virtual ~NodeTerritory();
			virtual void render();
			virtual void update(float updateFactor);
		
			virtual void updatePathTo(Node *goal, int dist);

			virtual void onTaskAccepted(Task *t);
			virtual void onTaskAborted(Task *t);
			virtual void onTaskFinished(Task *t);

			void updateConqueringProgressBy(int player, int amount);

			void setOwner(int i);
			int getOwner();

			virtual int getCreationPacketSize();
			virtual void writeCreationPacket(unsigned char *buff);
			virtual PACKET_TYPE getCreationPacketType();
		private:
			void updateTaskState();

			int owner;
			int freeJobs;
			std::map<int, int> conqueringProcess;

	};

	NodeTerritory *createNodeTerritoryFromPacket(unsigned char *buff);
}

#endif

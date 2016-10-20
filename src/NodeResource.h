#ifndef NODE_RESOURCE_H
#define NODE_RESOURCE_H

#include "Node.h"
#include "unistd.h"
#include "ResourceStorage.h"
#include "NetworkHelper.h"


namespace ant{

	class NodeResource : public Node, public ResourceStorage{

		public:
			NodeResource(int x, int y, int resourceType, bool onServer);
			NodeResource(int x, int y, int resourceType, unsigned int id, bool onServer);

			virtual ~NodeResource();
			virtual void render();
			virtual void update(float updateFactor);
			int getResourceType();
			virtual int storeResource(int resource, unsigned int amount);	
			virtual int getResources(int resource, unsigned int amount);
			virtual void updatePathTo(Node *goal, int dist);


			virtual void connectTo(Node *n);

			virtual void onTaskAccepted(Task *t);
			virtual void onTaskAborted(Task *t);
			virtual void onTaskFinished(Task *t);

			virtual int getCreationPacketSize();
			virtual void writeCreationPacket(unsigned char *buff);
			virtual PACKET_TYPE getCreationPacketType();
			
      virtual bool canBuildPathFrom();
			
		private:
			int resourceType, cooldown, freeResources, exhaustionCooldown;
			bool connected, hasFreeTask;
			void updateTaskState();
	};


	NodeResource *createNodeResourceFromPacket(unsigned char *buff);

}
#endif

#ifndef ANT_NODE_H
#define ANT_NODE_H

#include <unistd.h>
#include <vector>
#include <map>
#include "Task.h"
#include "NetworkHelper.h"


namespace ant{

	class Ant;
        class NetAnt;

	struct nodeDirection{
		Node *next;
		int dist;
	};

	struct NodeConnection{
			int useCount = 0;
			float speedMultiplier = 1;
			int users = 0;
			float length = 0;
			unsigned int creatorId = -1;
	};

	class Node : public TaskProvider{

		public:
			Node(bool onServer);
			Node(int x, int y, bool onServer);
			Node(int x, int y, unsigned int id, bool onServer);
			virtual ~Node();
			virtual void render();
      virtual void renderVision();
			int getX();
			int getY();
			virtual void connectTo(Node *n);
			void disconnectFrom(Node *n);
			void isolate();
			virtual void updatePathTo(Node *goal, int dist);
			void setPathTo(Node *goal, Node *next, int dist);
			int getDistanceTo(Node *n);
			unsigned int getId();
			bool isDeletable();
      virtual bool canBuildPathFrom();
			void removeDirection(Node *goal);
			bool searchForDirection(Node *goal, std::vector<Node *> *searched);
			virtual void update(float updateFactor);
			Node *getDirectionTo(Node *n);


			virtual void onTaskAccepted(Task *t);
			virtual void onTaskAborted(Task *t);
			virtual void onTaskFinished(Task *t);

			void addGoingTo(Ant *a);
			void removeGoingTo(Ant *a);
			void addComingFrom(Ant *a);
			void removeComingFrom(Ant *a);

			void attachAnt(NetAnt *a);
			void deattachAnt(NetAnt *a);

			Node *getNeighbor(int i);
			int getNeighborCount();
			bool isInteresting();
			float getSpeedMultiplierTo(Node *n);
			void setCombatPoint(bool b);
			bool isCombatPoint();



			virtual int getCreationPacketSize(){return 0;}
			virtual void writeCreationPacket(unsigned char *buff){}
			virtual PACKET_TYPE getCreationPacketType(){return NEW_NODE_RESSOURCE;}
		protected:
			void setInteresting(bool b);
			void setDeletable(bool b);
      bool onServer;
      void setConnectionInfoTo(int id, NodeConnection *n);
      void setX(int x);
      void setY(int y);

    private:
      void updateCombatTask();

      int x, y;
			std::vector<Node*> neighbors;
			std::map<int, NodeConnection*> connections;
			std::map<unsigned int, nodeDirection> directions;
			bool interesting;
			bool deletable;
			unsigned int id;
			std::vector<Ant *> antsComing; // List of all ants which have this Node as their goal
			std::vector<Ant *> antsGoing; // List of all ants which have this Node as their current
			std::vector<NetAnt *> antsAttached; //simplified List used on the server for combat computations
			float connectionUpdateCooldown;
			int freeTasks;
			bool combatPoint;
	};

}


#endif

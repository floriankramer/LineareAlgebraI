#ifndef ANT_WORLD_CLIENT_H
#define ANT_WORLD_CLIENT_H


#include "Node.h"
#include "NodeHive.h"
#include "Task.h"
#include "Ant.h"
#include "Border.h"

namespace ant{

	namespace world{


		void renderWorld();
		void updateWorld(float updateFactor);
		//void spawnPlayerAt(int x, int y);
		Node *createNodeAt( int x,  int y);
		void deleteNodeAt( int x,  int y);
		void setZoom(float f);
		void addTask(Task *t);
		Task *getTask(Ant *a);
		void deleteTask(Task *t);
	

		void spawnAnt(int x,  int y, int strength);
		void spawnAnt(Ant *a);

		void spawnNetAnt(int player, int id, int x, int y);
		void updateNetAnt(int player, int id, int x, int y, float dx, float dy);
		void killNetAnt(int player, int id);

		Node *getNodeById(int id);
		Node *getNodeAt( int x,  int y, int tolerance = 30);
		int getCenterX();
		int getCenterY();
		void killAnt(NetAnt *a);
		void cleanupWorld();
		bool intersectsBorder(int x1, int y1, int x2, int y2);
		int countBorderIntersections(int x1, int y1, int x2, int y2);
		int *getClosestIntersection(int x1, int y1, int x2, int y2);

		void setNumTerritories(int i);
		int getNumTerritories();

		void digTunnel(int x1, int y1, int x2, int y2, int width);

		NodeHive *getMainHive();

		void buildBorder(int id, int x1, int y1, int x2, int y2);
		void deleteBorder(int id);
		void addNode(Node *n);
		void registerHive(NodeHive *nh);
		void setMainHive(NodeHive *nh);

		void newPlayer(int id);
		void setPlayerId(int i);
		int getPlayerId();
		int getNewAntId();

    void setFogOFWarEnabled(bool b);
    bool isFogOfWarEnabled();

		std::vector<Border*>* getBorders();
		std::vector<Node*>* getImportantNodes();
		std::vector<NodeHive*>* getHives();
		std::map<int, std::map<int, NetAnt*>> *getAntMap();

	}

}

#endif

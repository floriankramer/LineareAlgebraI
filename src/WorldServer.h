#ifndef ANT_WORLD_SERVER_H
#define ANT_WORLD_SERVER_H

#include <vector>
#include <map>

namespace ant{

	class Node;
	class NodeHive;
	class Border;
	class NetAnt;

	namespace server{
		namespace world{
	
			void updateWorld(float updateFactor);
			//void spawnPlayerAt(int x, int y);
			Node *createNodeAt( int x,  int y);
			void deleteNodeAt( int x,  int y);
		
		
		
			void generateWorld(int numPlayers);

			NodeHive *getHiveForPlayer(int pId);
		

			void spawnNetAnt(int player, int id, int x, int y, int strength);
			void updateNetAnt(int player, int id, int x, int y, float dx, float dy, int currentNodeId);
			void killNetAnt(int player, int id);

			Node *getNodeById(int id);
			Node *getNodeAt( int x,  int y, int tolerance = 30);
			int getCenterX();
			int getCenterY();
			void killAnt(NetAnt *a);
			void cleanupWorld();
			bool intersectsBorder(int x1, int y1, int x2, int y2);	

			void buildBorder(int x1, int y1, int x2, int y2);
			void addNode(Node *n);

			void digTunnel(int x1, int y1, int x2, int y2, int width);

			void newPlayer(int id);		
			void removePlayer(int id);

			std::vector<Border*>* getBorders();
			std::vector<Node*>* getImportantNodes();
			std::vector<NodeHive*>* getHives();
			std::map<int, std::map<int, NetAnt*>> *getAntMap();

		}

	}

}

#endif

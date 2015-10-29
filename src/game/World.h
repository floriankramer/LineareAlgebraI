/*
 * World.h
 *
 *  Created on: Dec 13, 2014
 *      Author: dwarf
 */

#ifndef WORLD_H_
#define WORLD_H_

#include <cstdint>
#include <string>
#include "Entity.h"
#include <vector>
#include "EntityMinion.h"
#include "Node.h"

namespace game{
namespace world{

class World;

World* getCurrentWorld();

void freeMemory();
void renderBlocks(float updateFactor);
void renderNodes(bool editMode, float updateFactor);
void renderEntities(float updateFactor);
void renderNodeConnections(bool editMode, float updateFactor);
void renderWorldOutlines(float updateFactor);

void update(float updateFactor);
void updatePlatforms(float updateFactor);

bool reloadWorld();
World * loadWorld(std::string, int offsetX, int offsetY, bool updatePlayerPosition = false);
std::pair<int, int> getWorldSizeFromFile(std::string path);
void saveWorld(std::string);
void newWorld(int, int);
void clearWorlds();
bool unloadWorld(int x, int y, bool ignorePlayer = true);

void saveCampaign(std::string path);
void loadCampaign(std::string path);


void spawnEntity(Entity* e);
void removeEntity(Entity* e);
bool isTraversable(int, int);
float getGravity();
void renderNodes(bool editMode, float updateFactor);
void renderEntities(float updateFactor);
void updatePlatforms(float updateFactor);
void renderNodeConnections(bool editMode, float updateFactor);

/**
 * updates all entities and nodes
 */
std::vector<Entity*> * getEntities();
std::vector<EntityMinion*> * getMinions();
void addToMinions(EntityMinion *);
void removeFromMinions(EntityMinion *);

int addNode(Node* n);
Node *getNode(int i);
int getNodeCount();
void removeNode(int i);

bool withinWorld(int x, int y);

void setBlockAt(int x, int y, int type);
void setBlockTextureAt(int x, int y, int texture);
class World {

public:
	World();
	World(int width, int height);
	virtual ~World();
	std::uint8_t*** getBlocks();
	void setBlockAt(int x, int y, int i);
	void setBlockTextureAt(int x, int y, int i);
	void freeMemory();
	int getHeight();
	int getWidth();
	bool inBounds(int x, int y);
	int getBlockType(int x, int y);
	void setLocalOffsetX(int x);
	void setLocalOffsetY(int y);
	void renderBlocks();
	int getLocalOffsetX();
	int getLocalOffsetY();
	std::string getPath();
	void setPath(std::string path);
private:
	int width, height;
	long offsetX, offsetY;
	int localOffsetX, localOffsetY;
	std::uint8_t*** blocks;
	std::string path;
};

}
}

#endif



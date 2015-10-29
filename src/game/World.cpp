/*
 * World.cpp
 *
 *  Created on: Dec 13, 2014
 *      Author: dwarf
 */

#include "World.h"

#include "Physics.h"
#include "Game.h"
#include "GraphicsOptions.h"
#include "../warp/Renderer.h"
#include "../warp/Warp.h"
#include "LightHandler.h"
#include "CollisionHandler.h"
#include "NodeTypeMapper.h"
#include "ResourceLoader.h"
#include "RenderableSprite.h"


#include <algorithm>
#include <fstream>
#include <iostream>

namespace game{
namespace world{

std::vector<World *> worlds;

World *currentWorld = NULL;

World* getCurrentWorld(){
	return currentWorld;
}

void freeWorldMemory(){
	if(currentWorld != NULL){
		currentWorld->freeMemory();
	}
}

std::string lastWorld;

bool levelLoaded = false;

char theme;
std::vector<Entity*> entities;
std::vector<Entity *> entitiesToRemove;
std::vector<EntityMinion *> minions;
std::vector<Node *> nodes;

//float gravity = 0.6;
float gravity = 9.81;

std::map<int, render::RenderableSprite*> blockRenderables;

float getFloat(char *, int offset);
void writeFloat(float f, char *, int offset);

template<class T>
void logUInt(T i){

	std::string s;
	for(int j = sizeof(i) * 8 - 1; j >= 0; j--){
		if((i >> j) & 1){
			s += "1";
		}
		else{
			s += "0";
		}
	}
	gameLogger.log("UINT: " + s);
}

void newWorld(int x, int y){

	if(levelLoaded){ //delete the old block data, if it is present
		gameLogger.log("deleting old level");
		for(World * w : worlds){
			w->freeMemory();
			delete w;
		}
		worlds.clear();
		render::light::clearLights();
		for(Node *n : nodes){
			delete n;
		}
		for(Entity *e : entities){
			removeEntity(e);
		}

		entities.clear();
		nodes.clear();
		setUniqueIdCounter(0);
	}

	World * w = new World(x, y);


	//	width = x;
	//	height = y;
	//
	//	blocks = new uint8_t**[width];
	//	for(int x = 0; x < width; x++){
	//		blocks[x] = new uint8_t*[height];
	//		for(int y = 0; y < height; y++){
	//			blocks[x][y] = new uint8_t[2];
	//			blocks[x][y][0] = 0;
	//			blocks[x][y][1] = 0;
	//		}
	//	}

	worlds.push_back(w);

	levelLoaded = true;
}

void saveWorld(std::string location){ //TODO think about better multiworld support for saving

	if(worlds.size() == 0){
		return;
	}
	World * w = worlds[0];
	std::ofstream out;
	out.open(toOsFileString(location).c_str(), std::ios::out | std::ios::binary);

	char buffer[1024];

	//write Blocks
	buffer[0] = 1;
	out.write(buffer, 1);

	writeFloat(getPlayer()->getXCenter(), buffer, 0);
	writeFloat(getPlayer()->getYCenter(), buffer, 4);
	out.write(buffer, 8);

	buffer[0] = theme; //write theme...
	out.write(buffer, 1);

	buffer[0] = w->getWidth() >> 8;
	buffer[1] = w->getWidth();

	buffer[2] = w->getHeight() >> 8;
	buffer[3] = w->getHeight();

	out.write(buffer, 4);
	int bytesWritten = 0;

	for(int x = 0; x < w->getWidth(); x++){
		for(int y = 0; y < w->getHeight(); y++){
			buffer[bytesWritten] = w->getBlocks()[x][y][0];
			buffer[bytesWritten + 1] = w->getBlocks()[x][y][1];
			bytesWritten += 2;
			if(bytesWritten == sizeof(buffer)){
				out.write(buffer, sizeof(buffer));
				bytesWritten = 0;
			}
		}
	}
	out.write(buffer, bytesWritten);

	//write lights

	buffer[0] = 2;

	buffer[1] = render::light::getLightNumber() >> 8;
	buffer[2] = render::light::getLightNumber();

	out.write(buffer, 3);

	int written = 0;

	for(int i = 0; i < render::light::getLightNumber(); i++){
		render::light::Light *l = render::light::getLight(i);
		writeFloat(l->x, buffer, written);
		writeFloat(l->y, buffer, written + 4);
		writeFloat(l->strength, buffer, written + 8);
		written += 12;
		if(written > 1000){
			out.write(buffer, written);
			written = 0;
		}
	}

	out.write(buffer, written);


	//write nodes

	buffer[0] = 3;
	out.write(buffer, 1);

	uint32_t size = getNodeCount();
	memcpy(buffer, &size, sizeof(size));
	out.write(buffer, 4);

	bytesWritten = 0;
	uint16_t offset = 0;


	uint16_t id;

	for(Node *n : nodes){
		offset = 0;
		id = n->getUniqueId();
		buffer[bytesWritten + offset] = id << 8; offset ++;
		buffer[bytesWritten + offset] = id; offset ++;
		buffer[bytesWritten + offset] = n->getType(); offset ++;
		writeFloat(n->getXCenter(), buffer, bytesWritten + offset);offset += 4;
		writeFloat(n->getYCenter(), buffer, bytesWritten + offset);offset += 4;
		writeFloat(n->getWidth(), buffer, bytesWritten + offset);offset += 4;
		writeFloat(n->getHeight(), buffer, bytesWritten + offset);offset += 4;
		if(n->doesMove()){
			writeFloat(n->getInitX(), buffer, bytesWritten + offset); offset += 4;
			writeFloat(n->getInitY(), buffer, bytesWritten + offset); offset += 4;
			writeFloat(n->getGoalX(), buffer, bytesWritten + offset); offset += 4;
			writeFloat(n->getGoalY(), buffer, bytesWritten + offset); offset += 4;
		}

		//buffer[bytesWritten + offset] = n->getIngoingCount(); offset++;
		offset += n->writeIngoingConections(buffer, bytesWritten + offset);


		//		buffer[bytesWritten + offset] = n->getOutgoingCount() >> 8; offset ++;
		//		buffer[bytesWritten + offset] = n->getOutgoingCount(); offset ++;
		//		for(int i = 0; i < n->getOutgoingCount(); i++){
		//			id = n->getOutgoing(i)->getUniqueId();
		//			buffer[bytesWritten + offset] = id << 8; offset ++;
		//			buffer[bytesWritten + offset] = id; offset ++;
		//		}
		bytesWritten += offset;
		if(bytesWritten > 700){
			out.write(buffer, bytesWritten);
			bytesWritten = 0;
		}
	}
	out.write(buffer, bytesWritten);
	bytesWritten = 0;


	out.flush();
	out.close();

	lastWorld = location;
}

World* loadWorld(std::string location, int offsetX, int offsetY, bool updatePlayerPosition){
	World * w = NULL;
	std::ifstream in;
	in.open(toOsFileString(location.c_str()), std::ios::in | std::ios::binary);
	if(in.is_open()){
		gameLogger.log("loading level: " + location);
		char dataType;
		char buffer[1024];

		if(levelLoaded){ //delete the old block data, if it is present
			//			gameLogger.log("deleting old level");
			//			for(World * w : worlds){
			//				w->freeMemory();
			//				delete w;
			//			}
			//			worlds.clear();
			//			render::light::clearLights();
			//			for(Node *n : nodes){
			//				delete n;
			//			}
			//			for(Entity *e : entities){
			//				removeEntity(e);
			//				//				delete e;
			//			}
			//
			//			entities.clear();
			//			nodes.clear();
			//			setUniqueIdCounter(0);
		}

		in.read(&dataType, 1);
		std::streamsize bytesRead = in.gcount();
		while(bytesRead == 1){
			if(dataType == 1){
				gameLogger.log("found block data");
				in.read(buffer, 8);
				float playerX;
				playerX = getFloat(buffer, 0);
				float playerY;
				playerY = getFloat(buffer, 4);
				render::setCameraPosition(playerX, playerY);
				if(worlds.size()  == 0 || updatePlayerPosition){
					getPlayer()->setPositionCenter(playerX + offsetX, playerY + offsetY);
				}

				in.read(&theme, 1);


				in.read(buffer, 4); //read the new width and height from the file stream
				uint16_t width = ((static_cast<uint16_t>(buffer[0]) << 8) & 0xFF00) + (static_cast<uint16_t>(buffer[1]) & 0xFF);
				uint16_t height = ((static_cast<uint16_t>(buffer[2]) << 8) & 0xFF00) + (static_cast<uint16_t>(buffer[3]) & 0xFF);
				gameLogger.log("level width: " + std::to_string(width));
				gameLogger.log("level height: " + std::to_string(height));

				//				blocks = new uint8_t**[width];
				w = new World(width, height);
				w->setLocalOffsetX(offsetX);
				w->setLocalOffsetY(offsetY);
				w->setPath(location);
				//				for(int x = 0; x < width; x++){
				//					blocks[x] = new uint8_t*[height];
				//					for(int y = 0; y < height; y++){
				//						blocks[x][y] = new uint8_t[2];
				//						blocks[x][y][1] = 0;
				//					}
				//				}
				levelLoaded = true;

				std::map<int, bool> blockTexturesToLoad;

				int bytesToRead;
				bytesToRead = width * height * 2;
				int bytesReadTotal;
				bytesReadTotal = 0;
				while(bytesToRead > 0){
					int read = sizeof(buffer);
					if(read > bytesToRead)
						read = bytesToRead;
					in.read(buffer, read);
					bytesRead = in.gcount();
					bytesToRead -= bytesRead;
					for(int i = 0; i < bytesRead; i++){
						int h = bytesReadTotal / 2;
						//blocks[h / height][h % height][bytesReadTotal % 2] = buffer[i];
						w->getBlocks()[h / height][h % height][bytesReadTotal % 2] = buffer[i];
						if(bytesReadTotal % 2 == 1){
							blockTexturesToLoad[buffer[i]] = true;
						}
						bytesReadTotal ++;
					}
				}

				for(auto a : blockTexturesToLoad){
					loadBlock(a.first);
				}
				worlds.push_back(w);
			}
			else if(dataType == 2){
				gameLogger.log("found light data");
				//				in.read(buffer, 2);
				//				in.ignore(((buffer[0] << 8) + buffer[1] * 12));
				in.read(buffer, 2);
				int num = (buffer[0] << 8) + buffer[1];
				gameLogger.log(std::to_string(num) + " lights to load");
				for(int j = 0; j < num; j++){
					in.read(buffer, 12);
					render::light::addLight(getFloat(buffer, 0) + offsetX, getFloat(buffer, 4) + offsetY, getFloat(buffer, 8));
				}
			}
			else if(dataType == 3){
				gameLogger.log("found node data");

				std::streamsize bytesReadNode = 0;

				in.read(buffer, sizeof(buffer));
				bytesReadNode = in.gcount();

				uint32_t count;
				memcpy(&count, buffer, sizeof(count));
				gameLogger.log(std::to_string(count) + " nodes to load.");
				uint16_t offset = 4;
				uint16_t id;
				//				uint16_t outgoingCount = 0;
				int maxUUID = 0;

				int uidOffset = game::getUniqueIdCounter();
				for(unsigned int i = 0; i < count; i++){
					Node *n;

					id = (buffer[offset] << 8) + buffer[offset + 1] + uidOffset; offset += 2;
					n = nodeTypes[buffer[offset]](); offset ++;
					n->setUniqueId(id);
					if(id > maxUUID)
						maxUUID = id;
					n->setXCenter(getFloat(buffer, offset) + offsetX);offset += 4;
					n->setYCenter(getFloat(buffer, offset) + offsetY);offset += 4;
					n->setWidth(getFloat(buffer, offset));offset += 4;
					n->setHeight(getFloat(buffer, offset));offset += 4;
					if(n->doesMove()){
						n->setInitX(getFloat(buffer, offset) + offsetX); offset += 4;
						n->setInitY(getFloat(buffer, offset) + offsetY); offset += 4;
						n->setGoalX(getFloat(buffer, offset) + offsetX); offset += 4;
						n->setGoalY(getFloat(buffer, offset) + offsetY); offset += 4;
						n->recalculateMotionData();
					}

					offset += n->readIngoingConections(buffer, offset, uidOffset);

					//					outgoingCount = (buffer[offset] << 8) + buffer[offset + 1]; offset += 2;
					//					for(unsigned int h = 0; h < outgoingCount; h++){
					//						id = (buffer[offset] << 8) + buffer[offset + 1]; offset += 2;
					//						n->addTempOutgoingId(id);
					//					}

					if(offset > 512 && bytesReadNode > 512){
						memcpy(buffer, buffer + 512, 512);
						in.read(buffer + 512, 512);
						bytesReadNode = in.gcount();
						offset -= 512;
					}
					addNode(n);

				}

				game::setUniqueIdCounter(maxUUID);

				for(Node *n : nodes){
					n->resolveIngoingConnections();
				}

			}
			else{
				gameLogger.log("found unknown level data. aborting");
				in.close();
				return w;
			}


			in.read(&dataType, 1);
			bytesRead = in.gcount();
		}

		in.close();
	}
	else{
		gameLogger.log(std::string("Error while opening level file: ") + location);
		return w;
	}


	gameLogger.log("loaded level: " + location);
	lastWorld = location;
	return w;

}

std::pair<int, int> getWorldSizeFromFile(std::string location){
	std::ifstream in;
	in.open(location.c_str(), std::ios::in | std::ios::binary);
	if(in.is_open()){
		char dataType;
		char buffer[1024];
		in.read(&dataType, 1);
		std::streamsize bytesRead = in.gcount();
		while(bytesRead == 1){
			if(dataType == 1){
				in.read(buffer, 8);
				in.read(&theme, 1);
				in.read(buffer, 4); //read the new width and height from the file stream
				uint16_t width = ((static_cast<uint16_t>(buffer[0]) << 8) & 0xFF00) + (static_cast<uint16_t>(buffer[1]) & 0xFF);
				uint16_t height = ((static_cast<uint16_t>(buffer[2]) << 8) & 0xFF00) + (static_cast<uint16_t>(buffer[3]) & 0xFF);
				return std::pair<int, int>(width, height);
				in.close();
			}
			else{
				gameLogger.log("block data not at beginning; aborting");
				in.close();
				return std::pair<int, int>(0, 0);
			}
			in.read(&dataType, 1);
			bytesRead = in.gcount();
		}
		in.close();
	}
	else{
		gameLogger.log(std::string("Error while opening level file: ") + location);
		return std::pair<int, int>(0, 0);
	}
	return std::pair<int, int>(0, 0);
}

bool reloadWorld(){
	for(World * w : worlds){
		if(w->inBounds(game::getPlayer()->getXCenter(), game::getPlayer()->getYCenter())){
			int oX = w->getLocalOffsetX(); //TODO for dynamic world loading, use offset not local offset
			int oY = w->getLocalOffsetY();

			std::vector<Entity *> etd;
			for(EntityMinion *m : minions){
				etd.push_back(m);
			}
			for(Entity *e : etd){
				removeEntity(e);
			}

			std::string path = w->getPath();
			unloadWorld(w->getLocalOffsetX(), w->getLocalOffsetY());
			loadWorld(path, oX, oY, true);

			return true;
		}
	}


	return false;
}

void clearWorlds(){
	gameLogger.log("deleting old level");
	for(World * w : worlds){
		w->freeMemory();
		delete w;
	}
	worlds.clear();
	render::light::clearLights();
	for(Node *n : nodes){
		delete n;
	}
	for(Entity *e : entities){
		removeEntity(e);
		//				delete e;
	}

	entities.clear();
	nodes.clear();
	setUniqueIdCounter(0);
}

bool unloadWorld(int x, int y, bool ignorePlayer){
	for(unsigned int i = 0; i < worlds.size(); i++){
		World * w = worlds[i];
		if(w->inBounds(x, y)){
			if(!ignorePlayer && w->inBounds(game::getPlayer()->getXCenter(), game::getPlayer()->getYCenter())){
				return false;
			}
			w->freeMemory();
			worlds.erase(std::remove(worlds.begin(), worlds.end(), w), worlds.end());
			return true;
		}
	}
	return false;
}

void saveCampaign(std::string path){
	std::ofstream out;
	out.open(toOsFileString(path).c_str(), std::ios::out | std::ios::binary);
	if(out.is_open()){
		for(World * w : worlds){
			out << static_cast<uint32_t>(w->getLocalOffsetX()) << " " << static_cast<uint32_t>(w->getLocalOffsetY()) << " " << w->getPath() << "\n";
		}
		out.close();
		gameLogger.log("wrote campaign to: " + path);
	}
	else{
		gameLogger.log("unable to save campaign file at: " + path);
	}

}



void loadCampaign(std::string path){
	std::ifstream in;
	std::string line;
	in.open(toOsFileString(path).c_str(), std::ios::in | std::ios::binary);
	if(in.is_open()){
		clearWorlds();
		while (std::getline(in, line)){
			if(line.length() > 0){
				int pos = 0, oldPos = 0;
				pos = line.find(" ", oldPos);
				int x = std::stoi(line.substr(oldPos, pos));
				oldPos = pos + 1;
				pos = line.find(" ", oldPos);
				int y = std::stoi(line.substr(oldPos, pos - oldPos));
				loadWorld(line.substr(pos + 1, line.length()), x, y);
			}
		}
		game::physics::setPaused(false);
		in.close();
	}


}

void addToMinions(EntityMinion *e){
	minions.push_back(e);
}
void removeFromMinions(EntityMinion *e){
	minions.erase(std::remove(minions.begin(), minions.end(), e), minions.end());
}



void renderBlocks(float updateFactor){
	if(levelLoaded){

		for(World * w : worlds){
			w->renderBlocks();
		}

		//		float camX = game::getCameraX();
		//		float camY = game::getCameraY();
		//		float vwidth = render::options::getVirtualWidth();
		//		float vheight = render::options::getVirtualHeight();
		//		int minX = (int)(camX - vwidth) - 2;
		//		if(minX < 0)
		//			minX = 0;
		//		int maxX = (int)(camX + vwidth) + 2;
		//		if(maxX > width)
		//			maxX = width;
		//		int minY = (int)(camY - vheight) - 2;
		//		if(minY < 0)
		//			minY = 0;
		//		int maxY = (int)(camY + vheight) + 2;
		//		if(maxY > height)
		//			maxY = height;
		//
		//		for(int x = minX; x < maxX; x ++){
		//			for(int y = minY; y < maxY; y ++){
		//				if(blocks[x][y][0] == 1){
		//					if(blockRenderables[blocks[x][y][1]] == NULL){
		//						blockRenderables[blocks[x][y][1]] = render::createRenderableSprite(0, 0, 0, 0, 0, "block" + std::to_string(blocks[x][y][1]));
		//						blockRenderables[blocks[x][y][1]]->setSustain(true);
		//						render::drawRenderable(blockRenderables[blocks[x][y][1]]);
		//						render::drawRenderable(blockRenderables[blocks[x][y][1]]->cloneWithTransforms(x + 0.5, y + 0.5, 1, 1, 0));
		//					}
		//					else{
		//						render::drawRenderable(blockRenderables[blocks[x][y][1]]->cloneWithTransforms(x + 0.5, y + 0.5, 1, 1, 0));
		//					}
		//					//					render::drawSprite(x + 0.5, y + 0.5, 1, 1, 0, "block" + std::to_string(blocks[x][y][1]));
		//				}
		//			}
		//		}
	}
}

void renderWorldOutlines(float updateFactor){
	for(World * w : worlds){
		render::drawRectOutline(w->getLocalOffsetX() + w->getWidth() / 2.0, w->getLocalOffsetY() + w->getHeight() / 2.0, w->getWidth(), w->getHeight(), 0, 0.1);
	}
}


void renderEntities(float updateFactor){
	for(Entity* e : entities){
		e->render(updateFactor);
	}
}

void renderNodes(bool editMode, float updateFactor){
	for(Node *n : nodes){
		n->render(editMode, updateFactor);
	}
}

void renderNodeConnections(bool editMode, float updateFactor){
	if(editMode){
		for(Node *n : nodes){
			n->renderConnections(updateFactor);
		}
	}
}

bool withinWorld(int x, int y){
	for(World * w : worlds){
		if(w->inBounds(x, y)){
			return true;
		}
	}
	return false;
}

void update(float updateFactor){
	for(Entity* e : entities){
		e->doGameUpdate(updateFactor);
	}

	for(Entity* e : entities){
		e->applyGravity(gravity);
		e->handleMovementX(updateFactor);
		e->handleMovementY(updateFactor);
		e->resetForces();
	}

	for(Entity* e : entities){
		for(Node *n : nodes){
			//if(game::physics::pointInRect(e->getXCenter(), e->getYCenter(), n->getX(), n->getY(), n->getWidth(), n->getHeight())){
			if(game::physics::rectsOverlap(e->getX(), e->getY(), e->getWidth(), e->getHeight(), n->getX(), n->getY(), n->getWidth(), n->getHeight())){
				n->activate(COLLISION, e);
			}
		}
	}

	for(Node *n : nodes){
		//if(game::physics::pointInRect(game::getPlayer()->getXCenter(), game::getPlayer()->getYCenter(), n->getX(), n->getY(), n->getWidth(), n->getHeight())){
		if(game::physics::rectsOverlap(game::getPlayer()->getX(), game::getPlayer()->getY(), game::getPlayer()->getWidth(), game::getPlayer()->getHeight(), n->getX(), n->getY(), n->getWidth(), n->getHeight())){
			n->activate(COLLISION, game::getPlayer());
		}
		n->doGameUpdate(updateFactor);
		if(n->doesMove() && !n->doesCollide()){
			n->move(updateFactor);
		}
	}

	for(Entity *e : entitiesToRemove){
		for(unsigned int i = 0; i < entities.size(); i++){
			if(entities.at(i) == e)
				entities.erase(entities.begin() + i);
		}
		delete e;
	}

	entitiesToRemove.clear();

	//	entityList.removeAll(entitiesToRemove);
	//	entitiesToRemove.clear();
}

float getFloat(char * buffer, int offset){
	float f;
	int32_t i;
	memcpy(&i, buffer + offset, 4);
	f = i / 1000.0;
	return f;
}

void writeFloat(float f, char * b, int offset){
	int32_t i = f * 1000;
	memcpy(b + offset, &i, 4);

}

void spawnEntity(Entity* e){
	entities.push_back(e);
	e->onSpawn();
}

void setBlockAt(int x, int y, int type){
	for(World * w : worlds){
		if(w->inBounds(x, y)){
			w->setBlockAt(x, y, type);
		}
	}
}

void setBlockTextureAt(int x, int y, int texture){
	for(World * w : worlds){
		if(w->inBounds(x, y)){
			w->setBlockTextureAt(x, y, texture);
		}
	}
}

void removeEntity(Entity* e){
	if(e != NULL){
		if(e != getPlayer()){
			if(!e->isDead()){
				e->onDeath();
				e->kill();
				entitiesToRemove.push_back(e);
			}
		}
		else{
			physics::setPaused(true);
		}
	}
	else{
		gameLogger.log("tried to delete NULL entity");
		gameLogger.logCurrentStackTrace();
	}
}

bool isTraversable(int x, int y){
	for(World* w : worlds){
		if(w->inBounds(x, y)){
			return w->getBlockType(x, y) == 0;
		}
	}
	return true;
}

float getGravity(){
	return gravity;
}

std::vector<Entity*>* getEntities(){
	return &entities;
}

std::vector<EntityMinion*>* getMinions(){
	return &minions;
}

void updatePlatforms(float updateFactor){
	for(Node * n : nodes){
		if(n->doesMove() && n->doesCollide()){
			n->move(updateFactor);
			if(n->getMotionX() != 0 || n->getMotionY() != 0){
				Entity *e = getPlayer();
				unsigned int i = -1;
				do{

					if(physics::rectsOverlap(e->getX(), e->getY(), e->getWidth(), e->getHeight(), n->getX(), n->getY(), n->getWidth(), n->getHeight())){
						if(!e->move(n->getMotionX(), n->getMotionY())){
							removeEntity(e);
						}
					}
					else if(physics::atop(e->getX(), e->getY(), e->getWidth(), e->getHeight(), n->getX(), n->getY(), n->getWidth(), n->getHeight(), 0.01)){
						if(!e->move(n->getMotionX(), 0)){
							e->move(-n->getMotionX(), 0);
						}
					}
					i++;
					if(i < entities.size())
						e = entities[i];
				}while(i < entities.size());
			}

		}
	}
}

int addNode(Node* n){
	nodes.push_back(n);
	return nodes.size() - 1;
}
Node *getNode(int i){
	return nodes.at(i);
}
int getNodeCount(){
	return nodes.size();
}
void removeNode(int i){
	Node * r = nodes.at(i);
	for(Node* n : nodes){
		n->removeConnectionsTo(r);
	}
	delete r;
	nodes.erase(nodes.begin() + i);
}


void freeMemory(){
	if(levelLoaded){ //delete the old block data, if it is present
		gameLogger.log("deleting old level");
		for(World * w : worlds){
			w->freeMemory();
		}
		for(Node *n : nodes){
			delete n;
		}
		for(Entity *e : entities){
			delete e;
		}

		entities.clear();
		nodes.clear();render::light::clearLights();
	}
}

World::World() : width(0), height(0), offsetX(0), offsetY(0), localOffsetX(0), localOffsetY(0), blocks(NULL), path(""){

}

World::World(int width, int height) : width(width), height(height), offsetX(0), offsetY(0), localOffsetX(0), localOffsetY(0), path(""){
	//blocks = new uint8_t[width][height][2];
	blocks = new uint8_t**[width];
	for(int x = 0; x < width; x++){
		blocks[x] = new uint8_t*[height];
		for(int y = 0; y < height; y++){
			blocks[x][y] = new uint8_t[2];
			blocks[x][y][0] = 0;
			blocks[x][y][1] = 0;
		}
	}
}

World::~World(){

}

void World::freeMemory(){
	for(int x = 0; x < width; x++){
		for(int y = 0; y < height; y++){
			delete[] blocks[x][y];
		}
		delete[] blocks[x];
	}
	delete[] blocks;

	for(int i = 0; i < render::light::getLightNumber(); i++){
		render::light::Light *l = render::light::getLight(i);
		if(inBounds(l->x, l->y)){
			render::light::removeLight(i);
			i--;
		}
	}

	for(unsigned int i = 0; i < nodes.size(); i++){
		Node *n = nodes[i];
		if(inBounds(n->getXCenter(), n->getYCenter())){
			removeNode(i);
			i--;
		}
	}

	for(unsigned int i = 0; i < entities.size(); i++){
		Entity *e = entities[i];
		if(inBounds(e->getXCenter(), e->getYCenter())){
			removeEntity(e);
		}
	}

}


void World::renderBlocks(){
	for(int x = 0; x < width; x ++){
		for(int y = 0; y < height; y ++){
			if(blocks[x][y][0] != 0){//TODO reset to == 1
				if(blockRenderables[blocks[x][y][1]] == NULL){
					blockRenderables[blocks[x][y][1]] = render::createRenderableSprite(0, 0, 0, 0, 0, "block" + std::to_string(blocks[x][y][1]));
					blockRenderables[blocks[x][y][1]]->setSustain(true);
					render::drawRenderable(blockRenderables[blocks[x][y][1]]);
					render::drawRenderable(blockRenderables[blocks[x][y][1]]->cloneWithTransforms(localOffsetX + x + 0.5, localOffsetY + y + 0.5, 1, 1, 0));
				}
				else{
					render::drawRenderable(blockRenderables[blocks[x][y][1]]->cloneWithTransforms(localOffsetX + x + 0.5, localOffsetY + y + 0.5, 1, 1, 0));
				}
				//					render::drawSprite(x + 0.5, y + 0.5, 1, 1, 0, "block" + std::to_string(blocks[x][y][1]));
			}
		}
	}
}

void World::setBlockAt(int x, int y, int i){
	//	if(x >= 0 && x < width && y >= 0 && y < height){
	blocks[x - localOffsetX][y - localOffsetY][0] = i;
	//	}
}
void World::setBlockTextureAt(int x, int y, int i){
	//	if(x >= 0 && x < width && y >= 0 && y < height){
	blocks[x - localOffsetX][y - localOffsetY][1] = i;
	//	}
}
int World::getHeight(){
	return height;
}
int World::getWidth(){
	return width;
}

uint8_t*** World::getBlocks(){
	return blocks;
}

bool World::inBounds(int x, int y){
	return x - localOffsetX >= 0 && x - localOffsetX < width && y - localOffsetY >= 0 && y - localOffsetY < height;
}

int World::getBlockType(int x, int y){
	if(x - localOffsetX >= 0 && x - localOffsetX < width && y - localOffsetY >= 0 && y - localOffsetY < height){
		return blocks[x - localOffsetX][y - localOffsetY][0];
	}
	return -1;
}

void World::setLocalOffsetX(int x){
	localOffsetX = x;
}

void World::setLocalOffsetY(int y){
	localOffsetY = y;
}

int World::getLocalOffsetX(){
	return localOffsetX;
}

int World::getLocalOffsetY(){
	return localOffsetY;
}

std::string World::getPath(){
	return this->path;
}

void World::setPath(std::string path){
	this->path = path;
}

}
}

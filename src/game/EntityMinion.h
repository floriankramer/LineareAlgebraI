/*
 * EntityMinion.h
 *
 *  Created on: Dec 21, 2014
 *      Author: dwarf
 */

#ifndef ENTITYMINION_H_
#define ENTITYMINION_H_

#include "EntityWalking.h"
#include <vector>

namespace game {


extern int minionCommand;

int getCommandDepth();
void setCommandDepth(int i);


class EntityMinion: public EntityWalking {
public:
	EntityMinion();
	virtual ~EntityMinion();
	virtual void render(float updateFactor);
	virtual void onDeath();
	virtual void onSpawn();
//	virtual void doCommandAt(std::string command, float x, float y);
	void decodeCommand(std::string answer);
//	std::vector<std::string> getCommandsAt(float x, float y);
	void doNextCommand();
	int getCommand(int i);
	void setCommand(int i, int c);
	void appendCommand(int c);
	bool getHasSpecial();
	void renderBasics(float updateFactor);
	void onCommandDepthChanged(unsigned int newCommandDepth);

private:
	bool canSeeLight();
	float *getSeenLight();
protected:
	void setHasSpecial(bool hasSpecial);

private:
//	std::vector<std::string> commands;
	std::vector<int> commands;
	bool hasSpecial;
	int commandPos;
	bool heavy;
};

int getMinionCount();
void setSelectedMinion(EntityMinion*);
EntityMinion* getSelectedMinion();

} /* namespace game */

#endif /* ENTITYMINION_H_ */

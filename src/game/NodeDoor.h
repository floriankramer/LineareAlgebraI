/*
 * NodeDoor.h
 *
 *  Created on: Feb 16, 2015
 *      Author: dwarf
 */

#ifndef GAME_NODEDOOR_H_
#define GAME_NODEDOOR_H_

#include <Node.h>

namespace game {

class NodeDoor: public game::Node {
public:
	NodeDoor();
	virtual ~NodeDoor();
	void onInputChanged(int handleId);
	void render(bool editMode, float updateFactor);
	virtual void move(float updateFactor);
private:
	float goalY;
	bool shouldMove;
	int soundId;
};

} /* namespace gui */

#endif /* GAME_NODEDOOR_H_ */

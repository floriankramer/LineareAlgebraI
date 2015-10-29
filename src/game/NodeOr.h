/*
 * NodeOr.h
 *
 *  Created on: Apr 27, 2015
 *      Author: dwarf
 */

#ifndef GAME_NODEOR_H_
#define GAME_NODEOR_H_

#include <Node.h>

namespace game {

class NodeOr: public game::Node {
public:
	NodeOr();
	virtual ~NodeOr();
	void render(bool editMode, float updateFactor);
	void onInputChanged(int handleId);
private:
	bool positive;
};

} /* namespace gui */

#endif /* GAME_NODEOR_H_ */

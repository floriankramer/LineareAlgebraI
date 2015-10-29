/*
 * NodeOr.h
 *
 *  Created on: Feb 19, 2015
 *      Author: dwarf
 */

#ifndef GAME_NODEXOR_H_
#define GAME_NODEXOR_H_

#include <Node.h>

namespace game {

class NodeXor: public game::Node {
public:
	NodeXor();
	virtual ~NodeXor();
	void render(bool editMode, float updateFactor);
	void onInputChanged(int handleId);
};

} /* namespace gui */

#endif /* GAME_NODEXOR_H_ */

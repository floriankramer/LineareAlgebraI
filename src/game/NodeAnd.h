/*
 * NodeAnd.h
 *
 *  Created on: Feb 19, 2015
 *      Author: dwarf
 */

#ifndef GAME_NODEAND_H_
#define GAME_NODEAND_H_

#include <Node.h>

namespace game {

class NodeAnd: public game::Node {
public:
	NodeAnd();
	virtual ~NodeAnd();
	void render(bool editMode, float updateFactor);
	void onInputChanged(int handleId);
private:
	bool positive;
};

} /* namespace gui */

#endif /* GAME_NODEAND_H_ */

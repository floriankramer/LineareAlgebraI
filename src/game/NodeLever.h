/*
 * NodeLever.h
 *
 *  Created on: Feb 18, 2015
 *      Author: dwarf
 */

#ifndef GAME_NODELEVER_H_
#define GAME_NODELEVER_H_

#include <Node.h>

namespace game {

class NodeLever: public game::Node {
public:
	NodeLever();
	virtual ~NodeLever();
	void activate(ActivationType a, void * data = NULL);
	void render(bool editMode, float updateFactor);
private:
	bool state;
};

} /* namespace gui */

#endif /* GAME_NODELEVER_H_ */

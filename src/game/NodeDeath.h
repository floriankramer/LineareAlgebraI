/*
 * NodeDeath.h
 *
 *  Created on: Feb 19, 2015
 *      Author: dwarf
 */

#ifndef GAME_NODEDEATH_H_
#define GAME_NODEDEATH_H_

#include <Node.h>

namespace game {

class NodeDeath: public game::Node {
public:
	NodeDeath();
	virtual ~NodeDeath();
	void render(bool editMode, float updateFactor);
	void activate(ActivationType a, void * data = NULL);
};

} /* namespace gui */

#endif /* GAME_NODEDEATH_H_ */

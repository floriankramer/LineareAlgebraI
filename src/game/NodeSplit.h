/*
 * NodeSplit.h
 *
 *  Created on: Mar 11, 2015
 *      Author: dwarf
 */

#ifndef GAME_NODESPLIT_H_
#define GAME_NODESPLIT_H_

#include <Node.h>

namespace game {

class NodeSplit: public Node {
public:
	NodeSplit();
	virtual ~NodeSplit();
	void render(bool editMode, float updateFactor);
	void onInputChanged(int handleId);
};

} /* namespace game */

#endif /* GAME_NODESPLIT_H_ */

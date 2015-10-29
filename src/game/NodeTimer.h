/*
 * NodeTimer.h
 *
 *  Created on: Mar 11, 2015
 *      Author: dwarf
 */

#ifndef GAME_NODETIMER_H_
#define GAME_NODETIMER_H_

#include <Node.h>

namespace game {

class NodeTimer: public Node {
public:
	NodeTimer();
	virtual ~NodeTimer();

	void render(bool editMode, float updateFactor);
	void doGameUpdate(float updateFactor);
	void onInputChanged(int handleId);
private:
	float delay, currentDelay;
	bool targetState, run;
};

} /* namespace game */

#endif /* GAME_NODETIMER_H_ */

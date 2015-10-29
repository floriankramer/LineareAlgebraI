/*
 * NodePressurePlate.h
 *
 *  Created on: Mar 9, 2015
 *      Author: dwarf
 */

#ifndef GAME_NODEPRESSUREPLATE_H_
#define GAME_NODEPRESSUREPLATE_H_

#include <Node.h>

namespace game {

class NodePressurePlate: public Node {
public:
	NodePressurePlate();
	virtual ~NodePressurePlate();
	void render(bool editMode, float updateFactor);
	void activate(ActivationType a, void * data = NULL);
	void doGameUpdate(float updateFactor);
	void onInputChanged(int handleId);
private:
	bool isDown;
	unsigned int lastDownTick;
	bool heavy;
};

} /* namespace game */

#endif /* GAME_NODEPRESSUREPLATE_H_ */

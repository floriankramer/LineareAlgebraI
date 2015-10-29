/*
 * EntityMinionHeavy.h
 *
 *  Created on: Aug 28, 2015
 *      Author: dwarf
 */

#ifndef ENTITYMINIONHEAVY_H_
#define ENTITYMINIONHEAVY_H_

#include <EntityMinion.h>

namespace game {

class EntityMinionHeavy: public game::EntityMinion {
public:
	EntityMinionHeavy();
	virtual ~EntityMinionHeavy();
	virtual void render(float updateFactor);
	virtual void doGameUpdate(float updateFactor);
};

} /* namespace gui */

#endif /* ENTITYMINIONHEAVY_H_ */

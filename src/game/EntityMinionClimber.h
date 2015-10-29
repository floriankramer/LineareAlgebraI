/*
 * EntityMinionClimber.h
 *
 *  Created on: Apr 6, 2015
 *      Author: dwarf
 */

#ifndef GAME_ENTITYMINIONCLIMBER_H_
#define GAME_ENTITYMINIONCLIMBER_H_

#include <EntityMinion.h>

namespace game {

class EntityMinionClimber: public EntityMinion {
public:
	EntityMinionClimber();
	virtual ~EntityMinionClimber();
	virtual void render(float updateFactor);
	virtual void doGameUpdate(float updateFactor);
};

} /* namespace render */

#endif /* GAME_ENTITYMINIONCLIMBER_H_ */

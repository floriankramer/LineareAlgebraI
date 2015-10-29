/*
 * NodeTypeMapper.h
 *
 *  Created on: Feb 18, 2015
 *      Author: dwarf
 */

#ifndef GAME_NODETYPEMAPPER_H_
#define GAME_NODETYPEMAPPER_H_

#include "Node.h"
#include "NodeLever.h"
#include "NodeDoor.h"
#include "NodeAnd.h"
#include "NodeXor.h"
#include "NodeDeath.h"
#include "NodePressurePlate.h"
#include "NodeValue.h"
#include "NodeTimer.h"
#include "NodeSplit.h"
#include "NodeOr.h"

namespace game{

template<typename T>
Node* instantiator() {
    return new T;
}
typedef Node* (*instantiator_ptr)();
static instantiator_ptr nodeTypes[] = {
    &instantiator<Node>,
    &instantiator<NodeLever>,
    &instantiator<NodeDoor>,
    &instantiator<NodeAnd>,
    &instantiator<NodeXor>,
	&instantiator<NodeDeath>,
	&instantiator<NodePressurePlate>,
	&instantiator<NodeValue>,
	&instantiator<NodeTimer>,
	&instantiator<NodeSplit>,
	&instantiator<NodeOr>
};

}


#endif /* GAME_NODETYPEMAPPER_H_ */

/*
 * physics.h
 *
 *  Created on: Dec 2, 2014
 *      Author: dwarf
 */

#ifndef PHYSICS_H
#define PHYSICS_H

#include "../Logger.h"


namespace game{



namespace physics{


extern Logger physicsLogger;


//void * run(void *);
void update();
float getSimulationSpeed();
void setSimulationSpeed(float);
bool isPaused();
void setPaused(bool p);

}
}

#endif



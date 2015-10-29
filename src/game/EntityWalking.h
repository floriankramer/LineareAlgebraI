/*
 * EntityWalking.h
 *
 *  Created on: Dec 21, 2014
 *      Author: dwarf
 */

#ifndef ENTITYWALKING_H_
#define ENTITYWALKING_H_

#include "Entity.h"

namespace game {

class EntityWalking : public Entity{
public:
	EntityWalking(float width, float height);
	virtual ~EntityWalking();

	virtual void doGameUpdate(float updateFactor);
	virtual void handleMovementX(float updateFactor);
	virtual void handleMovementY(float updateFactor);
	virtual void jump();
	virtual void setWalkingDirection(int d);
	void setJumpPower(float, float);
	void setJumpPowerX(float);
	void setJumpPowerY(float);
	bool isJumping();
	bool isHasReachedGoal();
	bool isClimbing();
	void setMovementSpeed(float s);
	float getMovementSpeed();
	int getWalkingDirection();
private:
	bool canJump, jumping;
	float goalX, goalY;
	bool hasReachedGoal;
	float jumpPowerY, jumpPowerX;
	int jumpDirection;
	bool climbing;
	float climbingX, climbingY;
	float climbingSpeedX, climbingSpeedY;
	float movementSpeed;

	int walkingDirection;
};

} /* namespace game */

#endif /* ENTITYWALKING_H_ */

/*
 * EntityPlayer.h
 *
 *  Created on: Dec 23, 2014
 *      Author: dwarf
 */

#ifndef ENTITYPLAYER_H_
#define ENTITYPLAYER_H_

#include "Entity.h"
#include "RenderableSprite.h"
#include "Character.h"


namespace game {


class EntityPlayer: public Entity {
public:
	EntityPlayer();
	virtual ~EntityPlayer();
	int getMaxMinions();
	int getMaxSlowDownPower();
	int getSlowDownPower();
	void render(float updateFactor);
	void handleMovementY(float updateFactor);
	void handleMovementX(float updateFactor);
	void doGameUpdate(float updateFactor);
	void jump();
	bool hasJumped();

	void setKeyJump(bool);
	void setKeyRun(bool);
	void setKeyMoveRight(bool);
	void setKeyMoveLeft(bool);

	bool getKeyJump();
	bool getKeyRun();
	bool getKeyMoveRight();
	bool getKeyMoveLeft();

private:
	bool jumped;
	int maxSlowDownPower;
	int slowDownPower;
	int maxMinions;

	bool climbing;
	float climbingX, climbingY;
	float climbingSpeedX, climbingSpeedY;

	bool keyMoveRight, keyMoveLeft, keyJump, keyRun;

//	render::RenderableSprite *renderable;

	void onCollideY(int direction);

	render::animation::Character *character;
};

} /* namespace game */

#endif /* ENTITYPLAYER_H_ */

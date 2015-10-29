/*
 * Entity.h
 *
 *  Created on: Dec 20, 2014
 *      Author: dwarf
 */

#ifndef ENTITY_H_
#define ENTITY_H_

#include <string>
#include <unistd.h>
#include "CollisionHandler.h"

namespace game {

class World;


class Entity {
public:
	Entity(float width, float height);
	virtual ~Entity();
	std::string texture;

	void applyGravity(float amount);
	void setPosition(float, float);
	void setPositionCenter(float, float);
	bool teleportTo(float, float);
	virtual void handleMovementX(float updateFactor);
	virtual void handleMovementY(float updateFactor);
	virtual bool canSpawn();
	virtual bool findSpawn(float);
	virtual void render(float updateFactor);
	virtual void doGameUpdate(float updateFactor);
	virtual void onDeath();
	virtual void onCollideX(int direction);
	virtual void onCollideY(int direction);
	virtual void onSpawn();
	//virtual void collidedWithLevelY(int);

	void setY(float);
	void setX(float);
	float getX();
	float getY();
	float getXCenter();
	float getYCenter();
	float getVelY();
	float getVelX();
	void setVelX(float);
	void setVelY(float);

	void setVelMX(float v);
	float getVelMX();

	float getWidth();
	float getHeight();
	physics::Rect getShape();
	int getFacing();

	void applyForce(float x, float y);
	void resetForces();

	/**
	 * moves the entity
	 * @returns true if the entity does not collide with anything after this move
	 */
	bool move(float x, float y);
	bool isDead();
	void kill();
	bool isHeavy();
	bool isCollider();
	bool isPusher();

protected:
	bool collidesWithNode(float xOffset, float yOffset);
	bool collidesWithWorld(float xOffset, float yOffset);
	bool collidesWithEntity(float xOffset, float yOffset);
	bool collides(float xOffset, float yOffset);
	void setHeavy(bool h);
	void setCollider(bool c);
	void setPusher(bool p);
private:
	const float maxSpeed = 50;
	float x, y, rot, width, height, velX, velY, fx, fy, mass, velMX;
	physics::Rect shape;
	int facing;
	bool dead;
	bool heavy;
	bool collider;
	bool pusher;
	float frictionCoefficient;





};

} /* namespace game */

#endif /* ENTITY_H_ */

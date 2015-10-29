/*
 * Entity.cpp
 *
 *  Created on: Dec 20, 2014
 *      Author: dwarf
 */

#include "Entity.h"


#include "Game.h"
#include "World.h"
#include "CollisionHandler.h"
#include "Physics.h"

namespace game {

Entity::Entity(float width, float height) : fx(0), fy(0), mass(1), velMX(0), facing(1), dead(0), heavy(0), collider(0), pusher(0), frictionCoefficient(1){
	this->width = width;
	this->height = height;
	x = 0;
	y = 0;
	rot = 0;
	velX = 0;
	velY = 0;
}

Entity::~Entity() {

}

/* namespace game */

void Entity::render(float updateFactor){

}

bool Entity::isDead(){
	return dead;
}
void Entity::kill(){
	dead = true;
}

void Entity::setVelX(float vx){
	velX = vx;
	if((velX + velMX) != 0){
		facing = (velX + velMX) > 0 ? 1 : -1;
	}
	if(velX > maxSpeed){
		velX = maxSpeed;
	}
	else if(velX < -maxSpeed){
		velX = -maxSpeed;
	}
}

void Entity::doGameUpdate(float updateFactor){

}

void Entity::onDeath(){

}

void Entity::setHeavy(bool h){
	heavy = h;
}

bool Entity::isHeavy(){
	return heavy;
}

void Entity::setCollider(bool b){
	collider = b;
}

bool Entity::isCollider(){
	return collider;
}

void Entity::setPusher(bool p){
	pusher = p;
}

bool Entity::isPusher(){
	return pusher;
}

void Entity::applyForce(float x, float y){
	fx += x;
	fy += y;
}

void Entity::setVelY(float vy){
	velY = vy;
	if(velY > maxSpeed){
		velY = maxSpeed;
	}
	else if(velY < -maxSpeed){
		velY = -maxSpeed;
	}
}

bool Entity::collidesWithWorld(float xOffset, float yOffset){
	for (int lx = (int)(x + xOffset); lx < (x + xOffset) + width + 1; lx++) {
		for (int ly = (int)(y + yOffset); ly < (y + yOffset) + height + 1; ly++) {
			if (!world::isTraversable(lx, ly)) { //world::getBlocks()[lx][ly][0]
				if (physics::rectsOverlap(x + xOffset, y + yOffset, width, height, lx, ly, 1, 1)) {
					return true;
				}
			}
		}
	}
	return false;
}

bool Entity::collidesWithNode(float xOffset, float yOffset){
	Node *n;
	bool collides = false;
	for(int i = 0; i < world::getNodeCount(); i++){
		n = world::getNode(i);
		if(n->doesCollide() && physics::rectsOverlap(x + xOffset, y + yOffset, width, height, n->getX(), n->getY(), n->getWidth(), n->getHeight())){
			collides = true;
			n->activate(COLLISION, this);
		}
	}
	return collides;
}

bool Entity::collidesWithEntity(float xOffset, float yOffset){
	bool collides = false;
	for(Entity * e : *world::getEntities()){
		if(e != this && e->isCollider() && physics::rectsOverlap(x + xOffset, y + yOffset, width, height, e->getX(), e->getY(), e->getWidth(), e->getHeight())){
			collides = true;
			if(pusher){ //TODO actually apply force? or at least use actual speed
				e->setVelX(3 * facing); //getVelX()
			}
			else{
				return true;
			}
		}
	}
	return collides;
}

bool Entity::collides(float xOffset, float yOffset){
	return collidesWithWorld(xOffset, yOffset) || collidesWithNode(xOffset, yOffset) || collidesWithEntity(xOffset, yOffset);
}

void Entity::onCollideX(int direction){

}

void Entity::onCollideY(int direction){

}

void Entity::applyGravity(float amount){
	//	velY -= amount;
	//	if(velY < -maxSpeed){
	//		velY = -maxSpeed;
	//	}
	fy -= amount * mass;
}

/**
 * sets the position ignoring possible collision
 * @param x
 * @param y
 */
void Entity::setPosition(float x, float y){
	this->x = x;
	this->y = y;
	shape.x = x;
	shape.y = y;
}

/**
 * sets the position ignoring possible collision
 * @param x
 * @param y
 */
void Entity::setPositionCenter(float x, float y){
	this->x = x - width / 2;
	this->y = y - height / 2;
	shape.x = this->x;
	shape.y = this->y;
}

/**
 * sets the position if the enitity can exists at the given position
 * @param x
 * @param y
 * @return
 */
bool Entity::teleportTo(float x, float y){

	if(!collidesWithWorld(-this->x + x, -this->y + y)){
		this->x = x;
		this->y = y;
		shape.x = this->x;
		shape.y = this->y;
		return true;
	}

	return false;
}

void Entity::handleMovementY(float updateFactor){
	fy -= velY * velY * frictionCoefficient;
	velY += updateFactor * fy / mass;
	fy = 0;
	if(velY < -maxSpeed){
		velY = -maxSpeed;
	}
	if(velY > maxSpeed){
		velY = maxSpeed;
	}

	if(velY != 0){
		float iter = velY * physics::getSimulationSpeed() * updateFactor;
		bool collided = false;
		int direction = 1;
		for (int i = 0; i < 8; i++) {
			if (collides(0, iter)) {
				collided = true;
				direction = velY > 0 ? 1 : -1;
				velY = 0;
				iter *= 0.5f;
			} else {
				y += iter;
				break;
			}
		}

		if(collided){
			onCollideY(direction);
		}

		//		if(y > world::getHeight()){ TODO readd removeal of entity outside of world
		//			world::removeEntity(this);
		//		}
		//		else if(y < 0){
		//			world::removeEntity(this);
		//		}
	}
}

void Entity::handleMovementX(float updateFactor){
	/*  TODO add proper friction calculation
	 *  Disable force calculations for now, as they are buggy
	if(collidesWithWorld(0, -0.1)){ //touches block?
		fx += velX * velX * frictionCoefficient * 10;
	}
	else{
		fx += velX * velX * frictionCoefficient;
	}
	if(velX == 0){
		if(collidesWithWorld(0, 0.1 * (fy / abs(fy)))){ // touches a block in the direction of fy
			if(fx > fy * 0.3){
				velX += updateFactor * fx / mass;
			}
		}
	}
	else{
		if(collidesWithWorld(0, 0.1 * (fy / abs(fy)))){ // touches a block in the direction of fy
			fx += abs(fy) * 0.6;
		}
		velX += updateFactor * fx / mass;
	}
*/

	if(velX + velMX != 0){
		bool collided = false;
		int direction = 1;
		float iter = (velX + velMX) * physics::getSimulationSpeed() * updateFactor;
		for (int i = 0; i < 4; i++) {
			if (!collides(iter, 0)) {
				x += iter;
				break;
			} else {
				if(!collided){
					direction = (velX + velMX) > 0 ? 1 : -1;
					collided = true;
				}
				velX = 0;
				iter *= 0.5;
			}
		}

		if(collided){
			onCollideX(direction);
		}

		//		if(x > world::getWidth()){
		//			world::removeEntity(this);
		//		}
		//		else if(x < 0){
		//			world::removeEntity(this);
		//		}

	}
}

void Entity::resetForces(){
	fx = fy = 0;
}

bool Entity::canSpawn(){
	return !collidesWithWorld(0, 0);
}

/**
 * searches for a spawn location near to the entities current location and then moves it there
 * @param distance
 * @return if a spawn has been found
 */
bool Entity::findSpawn(float distance){

	for(int oX = -1; oX < 2; oX ++){
		for(int oY = -1; oY < 2; oY ++){
			if(!collidesWithWorld(oX, oY)){
				x += oX;
				y += oY;
				return true;
			}
		}
	}

	return false;
}

void Entity::setX(float x){
	this->x = x;
	shape.x = this->x;
}

void Entity::setY(float y){
	this->y = y;
	shape.y = this->y;
}

float Entity::getX(){
	return x;
}

float Entity::getXCenter(){
	return x + width * 0.5f;
}

float Entity::getY(){
	return y;
}

float Entity::getYCenter(){
	return y + height * 0.5f;
}

float Entity::getVelX(){
	return velX;
}

float Entity::getVelY(){
	return velY;
}

float Entity::getWidth(){
	return width;
}
float Entity::getHeight(){
	return height;
}

void Entity::onSpawn(){

}

bool Entity::move(float x, float y){
	setX(getX() + x);
	setY(getY() + y);
	return !(collidesWithWorld(0, 0) || collidesWithNode(0, 0));
}

int Entity::getFacing(){
	return facing;
}

physics::Rect Entity::getShape(){
	return shape;
}

void Entity::setVelMX(float v){
	velMX = v;
	if((velX + velMX) != 0){
		facing = (velX + velMX) > 0 ? 1 : -1;
	}
}

float Entity::getVelMX(){
	return velMX;
}

}

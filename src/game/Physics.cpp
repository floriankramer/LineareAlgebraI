/*
 * physics.cpp
 *
 *  Created on: Dec 2, 2014
 *      Author: dwarf
 */

#include "Physics.h"

#include <SDL2/SDL.h>

#include "World.h"

#include "TextureLoader.h"
#include "Renderer.h"
#include "GraphicsOptions.h"
#include "main.h"

#include "Entity.h"
#include "EntityMinion.h"
#include "Game.h"
#include "Keyboard.h"
#include "AudioPlayer.h"

#include <iostream>

namespace game{
namespace physics{

bool paused;

bool isPaused(){
	return paused;
}

void setPaused(bool p){
	paused = p;
}


Logger physicsLogger("Physics");

float speed = 1;


void update(){

	if(!paused){

		EntityPlayer *player = getPlayer();
		audio::setListenerPosition(player->getX(), player->getY());

		world::updatePlatforms(render::getUpdateFactor());

		if (player->getVelY() == 0 && player->hasJumped()) {
			if (player->getKeyMoveRight()) {
				if(player->getKeyRun())
					player->setVelMX(10);
				else
					player->setVelMX(5);
			}
			else if (player->getKeyMoveLeft()) {
				if(player->getKeyRun())
					player->setVelMX(-10);
				else
					player->setVelMX(-5);
			}
			else {
				player->setVelMX(0);
			}
		}
		else{
			if (player->getKeyMoveRight()) {
				if(player->getVelX() < 1)
					player->setVelMX(1);
			}
			else if (player->getKeyMoveLeft()) {
				if(player->getVelX() > -1)
					player->setVelMX(-1);
			}
			else {
				player->setVelMX(0);
			}
		}

		getPlayer()->doGameUpdate(render::getUpdateFactor());
		getPlayer()->handleMovementX(render::getUpdateFactor());

//		if (player->getKeyJump() && player->hasJumped() && player->getVelY() == 0) { TODO talk about whether the player can or can't jump
//			player->jump();
//		}
		player->applyGravity(world::getGravity()); //getCurrentWorld()->getGravity()

		player->handleMovementY(render::getUpdateFactor());


		world::update(render::getUpdateFactor());

	}

}


float getSimulationSpeed(){
	return speed;
}

void setSimulationSpeed(float s){
	speed = s;
}



}
}

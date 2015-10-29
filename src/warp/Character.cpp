/*
 * Character.cpp
 *
 *  Created on: Mar 28, 2015
 *      Author: dwarf
 */

#include "Character.h"
#include "Renderer.h"
#include "Warp.h"

namespace render {
namespace animation{

Character::Character(): currentAnimation(NULL), currentTime(0), lastKeyFrame(0), deltaT(0), mainInstance(false){

}

Character::Character(Character *c) : name(c->getName()), animations(c->getAnimations()), bones(c->getBones()), currentAnimation(NULL), currentTime(0), lastKeyFrame(0), deltaT(0), mainInstance(false){

}

Character::~Character(){
	if(mainInstance){
		for(Animation2 *a: animations){
			delete a;
		}
	}
}

void Character::setAnimation(std::string name){
	for(unsigned int i = 0; i < animations.size(); i++){
		if(animations[i]->name == name){
			currentAnimation = animations[i];
			if(currentAnimation->keyframes.size() > 1){
				deltaT = currentAnimation->keyframes[1].first - currentAnimation->keyframes[0].first;
			}
		}
	}
}

void Character::updateAnimation(float updateFactor){
	if(currentAnimation != NULL && currentAnimation->keyframes.size() > 1){
		currentTime += updateFactor * 1000;
		if(currentAnimation->keyframes[lastKeyFrame + 1].first <= currentTime){
			lastKeyFrame ++;
			if(currentAnimation->keyframes.size() - 1 == lastKeyFrame){
				currentTime = 0;
				lastKeyFrame = 0;
			}
			deltaT = currentAnimation->keyframes[lastKeyFrame + 1].first - currentAnimation->keyframes[lastKeyFrame].first;
			if(deltaT <= 0){
				deltaT = 1;
			}
		}
		Key2 *k;
		float timeFac = ((currentTime - currentAnimation->keyframes[lastKeyFrame].first) / deltaT);
		for(unsigned int i = 0; i < currentAnimation->keyframes[lastKeyFrame].second.size(); i++){
			k = &currentAnimation->keyframes[lastKeyFrame].second[i];
			bones[k->boneId].rot = k->rot + (k->nextKey->rot - k->rot) * timeFac;
			bones[k->boneId].x = k->x + (k->nextKey->x - k->x) * timeFac;
			bones[k->boneId].y = k->y + (k->nextKey->y - k->y) * timeFac;
		}
		//		for(Key k : currentAnimation->keyframes[lastKeyFrame].second){
		//
		//		}


	}
}

void Character::addAnimation(Animation2 *a){
	animations.push_back(a);
}

void Character::addBone(Bone2 b){
	bones[b.id] = b;
}

void Character::render(float x, float y, int facing){
	for(unsigned int i = 0; i < bones.size(); i++){
		drawSprite(x + facing * bones[i].x, y + bones[i].y, bones[i].width * facing, bones[i].height, bones[i].rot + bones[i].rotOffset, bones[i].texture);
	}
//	drawRect(x, y, 0.5, 0.5, 0);
}

std::string Character::getName(){
	return name;
}

Character* Character::getCopy(){
	return new Character(this);
}

std::vector<Animation2 *> Character::getAnimations(){
	return animations;
}

std::map<int, Bone2> Character::getBones(){
	return bones;
}

void Character::setMainInstance(bool b){
	mainInstance = b;
}

void Character::setName(std::string name){
	this->name = name;
}

std::map<int, Bone2> *Character::getBonesPointer(){
	return &bones;
}

}
} /* namespace render */

/*
 * Character.h
 *
 *  Created on: Mar 28, 2015
 *      Author: dwarf
 */

#ifndef CHARACTER_H_
#define CHARACTER_H_

#include <vector>
#include <string>
#include <iostream>
#include <map>

namespace render {
namespace animation{

struct Bone2{
	int id;
	float x, y, width, height, rot;
	std::string texture;
	float xOffset, yOffset, rotOffset;

	void print(){
		std::cout << "\tid: " << id << std::endl;
		std::cout << "\tx, y: " << x << ", " << y << std::endl;
		std::cout << "\twidth, height: " << width << ", " << height << std::endl;
		std::cout << "\trot: " << rot << std::endl;
		std::cout << "\ttexture name: " << texture << "\n" << std::endl;
	}
};

struct Key2{
	int id;
	int timeline;
	int boneId;
	float x, y, width, height, rot;
	Key2 *nextKey;
	void print(){
		std::cout << "\tbone id: " << boneId << std::endl;
		std::cout << "\tx, y: " << x << ", " << y << std::endl;
		std::cout << "\twidth, height: " << width << ", " << height << std::endl;
		std::cout << "\trot: " << rot << "\n" << std::endl;
	}
};

struct Animation2{
	std::string name;
	int length;
	std::vector<std::pair<int, std::vector<Key2>>> keyframes;

	void print(){
		std::cout << "Animation: " << name << std::endl;
		std::cout << "Length: " << length << std::endl;
		for(auto a : keyframes){
			for(Key2 k : a.second){
				k.print();
			}
		}
	}
};

class Character{
public:
	Character();
	Character(Character *c);
	virtual ~Character();

	void setAnimation(std::string name);
	void addAnimation(Animation2 *a);
	void addBone(Bone2 b);
	void render(float x, float y, int facing);
	void updateAnimation(float updateFactor);
	std::string getName();
	void setName(std::string);
	Character *getCopy();
	void setMainInstance(bool b);
	std::map<int, Bone2> *getBonesPointer();

private:

	std::vector<Animation2 *> getAnimations();
	std::map<int, Bone2> getBones();

	std::string name;
	std::vector<Animation2 *> animations;
	std::map<int, Bone2> bones;
	Animation2 *currentAnimation;
	float currentTime;
	unsigned int lastKeyFrame;
	int deltaT;
	bool mainInstance;
};

}/* namespace animation */
} /* namespace render */

#endif /* CHARACTER_H_ */

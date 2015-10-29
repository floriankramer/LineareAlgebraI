/*
 * BlockTextureMap.h
 *
 *  Created on: Feb 21, 2015
 *      Author: dwarf
 */

#ifndef GAME_BLOCKTEXTUREMAP_H_
#define GAME_BLOCKTEXTUREMAP_H_

#include <map>
#include <string>

static const std::map<std::string, int> blockTextureMap = {
		{"stone", 0},
		{"brick", 1},
		{"cobblestone", 2},
		{"dirt", 3},
		{"grass", 4},
		{"plank", 5}
};



#endif /* GAME_BLOCKTEXTUREMAP_H_ */

/*
 * ResourceLoader.h
 *
 *  Created on: Feb 21, 2015
 *      Author: dwarf
 */

#ifndef GAME_RESOURCELOADER_H_
#define GAME_RESOURCELOADER_H_

#include <string>
#include "os.h"

namespace game{

bool loadResourceFile(std::string src);
void unloadResourceFile(std::string src);

bool loadBlock(std::string);
bool loadBlock(int id);

}



#endif /* GAME_RESOURCELOADER_H_ */

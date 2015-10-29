/*
 * ResourceLoader.cpp
 *
 *  Created on: Feb 21, 2015
 *      Author: dwarf
 */

#include "ResourceLoader.h"
#include <fstream>
#include <dirent.h>
#include "Game.h"
#include "TextureLoader.h"

#include "BlockTextureMap.h"

namespace game{

bool loadResourceFile(std::string s){
	//std::string src = toOsFileString(s);
	std::string src = s;
	struct dirent *entry;
	DIR *dp;

	dp = opendir(src.c_str());
	if (dp == NULL) {
		gameLogger.log("error while reading resource folder. Could not open folder");
		return false;
	}

	while ((entry = readdir(dp))){
		std::string file = std::string(entry->d_name);
		if(file.size() > 4 && file.substr(file.size() - 4, file.size()) == ".png"){
			render::loadTexture(file.substr(0, file.size() - 4), src + "/" + file);
		}
	}

	closedir(dp);
	return true;
}

bool loadBlock(int id){
	for(auto a : blockTextureMap){
		if(a.second == id){
			return loadBlock(a.first);
		}
	}
	return false;
}

bool loadBlock(std::string name){

	bool exists = false;
	int id = -1;
	for(auto a : blockTextureMap){
		if(a.first == name){
			exists = true;
			id = a.second;
			break;
		}
	}
	if(!exists)
		return false;

	return render::loadTexture("block" + std::to_string(id), "./gfx/blocks/" + name + ".png");
}

}




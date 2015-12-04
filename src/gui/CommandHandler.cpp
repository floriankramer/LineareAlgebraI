/*
 * CommandHandler.cpp
 *
 *  Created on: Jan 2, 2015
 *      Author: dwarf
 */

#include "CommandHandler.h"

#include "World.h"
#include "Physics.h"
#include "DisplayHandler.h"
#include "FontRenderer.h"
#include "GraphicsOptions.h"
#include "LightHandler.h"
#include "Game.h"
#include "Mouse.h"
#include "Warp.h"
#include "Widget.h"
#include <vector>
#include <string>
#include "ResourceLoader.h"
#include "BlockTextureMap.h"
#include "CoordinateConverter.h"
#include "EntityMinion.h"

#include <iostream>

namespace gui{

std::vector<std::string> parts;
std::string delimiter(" ");
std::vector<std::string> autocompleteBuffer;

std::string commands[] = {"setSpeed", "setFullscreen", "loadFont", "setResolution", "loadWorld", "setLightEnabled", "setEditMode", "newWorld", "saveWorld", "setPlayerPosition", "setCameraPosition", "setTool", "setPaused", "loadResourceFile", "loadBlock", "showFPS", "setFPS", "reloadWorld", "clearWorlds", "setToolParameter", "saveCampaign", "loadCampaign", "setCommandDepth", "rebuildShadowMap"};

bool startsWith(std::string base, std::string start){
	if(base.size() > start.size()){
		for(unsigned int i = 0; i < start.size(); i++){
			if(start[i] != base[i]){
				return false;
			}
		}
		return true;
	}
	return false;
}

std::string getPossibleCommands(std::string com){
	std::string t;
	for(std::string s : commands){
		if(startsWith(s, com)){
			t += s + " ";
		}
	}

	return t;
}

std::string autocomplete(std::string com){
	autocompleteBuffer.clear();
	for(std::string s : commands){
		if(startsWith(s, com)){
			autocompleteBuffer.push_back(s);
		}
	}
	if(autocompleteBuffer.size() == 1){
		return autocompleteBuffer.at(0);
	}
	else if(autocompleteBuffer.size() > 1){
		unsigned int min = 320000;
		for(std::string s : autocompleteBuffer){
			if(s.size() < min){
				min = s.size();
			}
		}
		std::string t = "";
		for(unsigned int i = 0; i < min; i++){
			char c = autocompleteBuffer.at(0)[i];
			for(std::string s : autocompleteBuffer){
				if(s[i] != c){
					return t;
				}
			}
			t += c;
		}
		return t;
	}
	return com;
}


std::string handleCommand(std::string command){
	std::string answer("unknown command");
	int pos = 0, oldPos = 0;
	while((pos = command.find(delimiter, oldPos + 1)) != -1){
		parts.push_back(command.substr(oldPos, pos - oldPos));
		oldPos = pos + 1;
	}
	parts.push_back(command.substr(oldPos, command.length() - oldPos));


	if(parts[0] == "loadWorld"){
		if(parts.size() > 1 && parts[1].length() > 0){
			game::world::World * w = NULL;
			if(parts.size() > 3){
				w = game::world::loadWorld(parts[1], std::stoi(parts[2]), std::stoi(parts[3]));
			}
			else{
				w = game::world::loadWorld(parts[1], 0, 0);
			}

			if(w != NULL){
				answer = "success";
			}
			else{
				answer = "error";
			}

			//			answer = "CommandHandler.cpp " + std::to_string(__LINE__) + " - not implemented";
		}
		else{
			answer = "usage: loadWorld [path]";
		}
	}
	else if(parts[0] == "saveWorld"){
		if(parts.size() > 1 && parts[1].length() > 0){
			game::world::saveWorld(parts[1]);
			answer = "done";
			//answer = "CommandHandler.cpp " + std::to_string(__LINE__) + " - not implemented";
		}
		else{
			answer = "usage: saveWorld [path]";
		}
	}
	else if(parts[0] == "clearWorlds"){
		game::world::clearWorlds();
		game::physics::setPaused(true);
		answer = "done";
	}
	else if(parts[0] == "setPlayerPosition"){
		float *f = gui::getMousePosition();
		f = screenToWorldSpace(f[0], f[1]);
		game::getPlayer()->setPositionCenter(f[0], f[1]);
		delete[] f;
		answer = "done";
	}
	else if(parts[0] == "setTool"){
		if(parts.size() > 1 && parts[1].length() > 0){
			answer = game::setTool(parts[1]);
		}
		else{
			answer = "usage: setTool [tool]";
		}
	}
	else if(parts[0] == "setCameraPosition"){
		if(parts.size() > 2 && parts[1].length() > 0 && parts[2].length() > 0){
			game::setCameraPosition(std::stof(parts[1]), std::stof(parts[2]));
		}
	}
	else if(parts[0] == "setEditMode"){
		if(parts.size() > 1 && parts[1].length() > 0){
			if(parts[1] == "true"){
				game::setEditMode(true);
				game::physics::setPaused(true);
				gui::setEditMode(true);
			}
			else{
				game::setEditMode(false);
				game::physics::setPaused(false);
				gui::setEditMode(false);
			}
			answer = "done";
		}
		else{//handle as true
			game::setEditMode(true);
			game::physics::setPaused(true);
			gui::setEditMode(true);
		}
	}
	else if(parts[0] == "setSpeed"){
		if(parts.size() > 1 && parts[1].length() > 0){
			game::physics::setSimulationSpeed(std::stof(parts[1]));
			answer = "done";
		}
		else{
			answer = "usage: setSpeed [float]";
		}
	}
	else if(parts[0] == "setFullscreen"){
		if(parts.size() > 1 && parts[1].length() > 0){
			if(parts[1] == "true"){
				render::setFullscreen(true);
			}
			else{
				render::setFullscreen(false);
			}
			answer = "done";
		}
		else{
			answer = "usage: setFullscreen [boolean]";
		}
	}
	else if(parts[0] == "loadFont"){
		if(parts.size() > 1 && parts[1].length() > 0){
			render::font::addFontToLoad(parts[1]);
			answer = "done";
		}
		else{
			answer = "usage: loadFont [path]";
		}
	}
	else if(parts[0] == "newWorld"){
		if(parts.size() > 2 && parts[1].length() > 0 && parts[2].length() > 0){
			game::world::newWorld(std::stoi(parts[1]), std::stoi(parts[2]));
			answer = "done";
			//answer = "CommandHandler.cpp " + std::to_string(__LINE__) + " - not implemented";
		}
		else{
			answer = "usage: newWorld [int width] [int height]";
		}
	}
	else if(parts[0] == "setResolution"){
		if(parts.size() > 2 && parts[1].length() > 0 && parts[2].length() > 0){
			render::options::setResolution(std::stoi(parts[1]), std::stoi(parts[2]));
			answer = "done";
		}
		else{
			answer = "usage: setResolution [int xres] [int yres]";
		}
	}
	else if(parts[0] == "setLightEnabled"){
		if(parts.size() > 1 && parts[1].length() > 0){
			if(parts[1] == "true"){
				render::light::setLightEnabled(true);
			}
			else{
				render::light::setLightEnabled(false);
			}
			answer = "done";
		}
		else{
			answer = "usage: setLightEnables [boolean]";
		}
	}
	else if(parts[0] == "setPaused"){
		if(parts.size() > 1 && parts[1].length() > 0){
			if(parts[1] == "true"){
				game::physics::setPaused(true);
			}
			else{
				game::physics::setPaused(false);
			}
			answer = "done";
		}
	}
	else if(parts[0] == "loadResourceFile"){
		if(parts.size() > 1 && parts[1].length() > 0){
			if(game::loadResourceFile(parts[1])){
				answer = "done";
			}
			else{
				answer = "error";
			}
		}
		else{
			answer == "usage: loadResourceFile [path]";
		}
	}
	else if(parts[0] == "loadBlock"){
		if(parts.size() > 1 && parts[1].length() > 0){
			if(parts[1] == "all"){
				answer = "done";
				for(auto a : blockTextureMap){
					if(game::loadBlock(a.first)){
						answer = "error";
					}
				}
			}
			else{
				if(game::loadBlock(parts[1])){
					answer = "done";
				}
				else{
					answer = "error";
				}
			}
		}
		else{
			answer == "usage: loadResourceFile [name / all]";
		}
	}
	else if(parts[0] == "showFPS"){
		if(parts.size() > 1 && parts[1].length() > 0){
			if(parts[1] == "true"){
				render::setShowFPSCounter(true);
			}
			else{
				render::setShowFPSCounter(false);
			}
			answer = "done";
		}
		else{
			answer = "usage: showFPS [boolean]";
		}
	}
	else if(parts[0] == "setFPS"){
		if(parts.size() > 1 && parts[1].length() > 0){
			render::options::setFPSCap(std::stoi(parts[1]));
			answer = "done";
		}
		else{
			answer = "usage: setFPS [int]";
		}
	}
	else if(parts[0] == "reloadWorld"){
		if(game::world::reloadWorld()){
			answer = "success";
		}
		else{
			answer = "error";
		}
		//		answer = "CommandHandler.cpp " + std::to_string(__LINE__) + " - not implemented";
	}
	else if(parts[0] == "setToolParameter"){
		if(parts.size() > 2){
			answer = game::setToolParameter(parts[1], parts[2]);
		}
		else{
			answer = "usage: setToolParameter [name] [value]";
		}
		//		answer = "CommandHandler.cpp " + std::to_string(__LINE__) + " - not implemented";
	}// "saveCampaign", "loadCampaign"
	else if(parts[0] == "saveCampaign"){
		if(parts.size() > 1){
			game::world::saveCampaign(parts[1]);
			answer = "done";
		}
		else{
			answer = "usage: saveCampaign [path]";
		}
	}
	else if(parts[0] == "loadCampaign"){
		if(parts.size() > 1){
			game::world::loadCampaign(parts[1]);
			answer = "done";
		}
		else{
			answer = "usage: loadCampaign [path]";
		}
	}
	else if(parts[0] == "setCommandDepth"){
		if(parts.size() > 1){
			int depth = std::stoi(parts[1]);
			if(depth >= 0){
				game::setCommandDepth(depth);
				answer = "done";
			}
			else{
				answer = "depth must be of type unsigned integer";
			}
		}
		else{
			answer = "usage: setCommandDepth [depth]";
		}
	}
	else if(parts[0] == "rebuildShadowMap"){
		render::light::rebuildShadowMap();
	}
	parts.clear();

	guiLogger.log("CONSOLE: " + command);
	guiLogger.log("CONSOLE: " + answer);

	return answer;
}

}

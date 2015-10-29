#include "main.h"

#include <iostream>
#include <SDL2/SDL.h>
#include <string>

#include <CollisionHandler.h>

#include "os.h"

std::string version = "0.0.2";

#ifdef unix
Thread physicsThread(game::run);
Thread renderThread(render::run);
#endif
#ifdef windows
Thread physicsThread(game::winRun);
Thread renderThread(render::winRun);
#endif

#ifdef windows
#include <tchar.h>
#undef main
int main(int argc, _TCHAR* argv[]){
	std::cout << "init" << std::endl;

	std::cout << "version: " << version << std::endl;

	renderThread.start();
	physicsThread.start();


	renderThread.joinCurrentWithThis();
	physicsThread.joinCurrentWithThis();

	std::cout << "exiting" << std::endl;
	return 0;
}
#endif

#ifdef unix
int main(int argc, char* argv[]){

	std::cout << "version: " << version << std::endl;

	//if(game::physics::lineIntersectsRect(-3, 3, 0.9, 0.9, -1, -1, 1, 1)){
//	if(game::physics::linesIntersect(-3, 3, 0.9, 0.9, -1, 1, -1, 1)){
//		std::cout << "intersecting\n";
//	}
//	else{
//		std::cout << "lines don't intersect\n";
//	}

	renderThread.start();
	physicsThread.start();


	renderThread.joinCurrentWithThis();
	physicsThread.joinCurrentWithThis();


	return 0;
}
#endif

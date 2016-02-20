#include "main.h"


#include <iostream>
#include <SDL2/SDL.h>
#include <string>
#include <CollisionHandler.h>

#include "os.h"

#ifdef unix
#include <gperftools/profiler.h>
#endif

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
//	ProfilerStart("/tmp/minion_game_profile"); //run googcle profiler
	std::cout << "version: " << version << std::endl;

	renderThread.start();
	physicsThread.start();


	renderThread.joinCurrentWithThis();
	physicsThread.joinCurrentWithThis();
//	ProfilerStop();

	return 0;
}
#endif

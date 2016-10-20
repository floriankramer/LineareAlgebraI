/*
 * DisplayHandler.h
 *
 *  Created on: Dec 11, 2014
 *      Author: dwarf
 */

#ifndef DISPLAYHANDLER_H_
#define DISPLAYHANDLER_H_

#include "SDL2/SDL.h"

namespace warp{

	void createDisplay();
	void destroyDisplay();
	void setDisplayResolution(int w, int h);
	void setFullscreen(bool);
	void updateFullscreenState();

	extern SDL_Window *window;
	extern SDL_GLContext glContext;
	extern float pixelWidth, pixelHeight;

}

#endif



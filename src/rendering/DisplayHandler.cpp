/*
 * DisplayHandler.cpp
 *
 *  Created on: Dec 11, 2014
 *      Author: dwarf
 */


//#include <GL/glew.h>

#include <string>

#include "glewWrapper.h"
#include <SDL2/SDL.h>

#include "DisplayHandler.h"
#include "Warp.h"
#include "GraphicsOptions.h"


namespace warp{

SDL_Window *window;
SDL_GLContext glContext;

float pixelHeight = 0;
float pixelWidth = 0;

bool shouldBeFullscreen = false;

void createDisplay(){



	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 8);


	warpLogger.log("creating window");
	if(options::isFullscreen())
		window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, options::getDisplayWidth(), options::getDisplayHeight(), SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP);
	else
		window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, options::getDisplayWidth(), options::getDisplayHeight(), SDL_WINDOW_OPENGL);

	pixelWidth = options::getVirtualWidth() / options::getDisplayWidth();
	pixelHeight = options::getVirtualHeight() / options::getDisplayHeight();

	warpLogger.log("creating openGL context");
	glContext = SDL_GL_CreateContext(window);

	GLenum err = glewInit();
	if(err != GLEW_OK){
		warpLogger.log("Error while initializing glew.");
	}
	char buffer[256];
	sprintf(buffer, "GLEW Version: %s", glewGetString(GLEW_VERSION));
	warpLogger.log(std::string(buffer));
	sprintf(buffer, "OpenGL Version: %s", glGetString(GL_VERSION));
	warpLogger.log(std::string(buffer));
	if(!GLEW_VERSION_3_0){
		warpLogger.log(std::string("OpenGL 3.0 is not available"));
		throw "unsupported OpenGL version";
	}


}

void setDisplayResolution(int w, int h){
	SDL_SetWindowSize(window, w, h);
}

void setFullscreen(bool fullscreen){
	shouldBeFullscreen = fullscreen;
}

void updateFullscreenState(){
	int i = 0;
	if(SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN_DESKTOP){
		if(!shouldBeFullscreen){
			i = SDL_SetWindowFullscreen(window, 0);
			if(i == 0){
				options::setFullscreen(false);
				SDL_SetWindowSize(window, options::getDisplayWidth(), options::getDisplayHeight());
				SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
			}
		}
	}
	else if(shouldBeFullscreen){
		i = SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		if(i == 0){
			options::setFullscreen(true);
			SDL_DisplayMode s;
			SDL_GetCurrentDisplayMode(0, &s);
			options::setFullscreenResolution(s.w, s.h);
		}

	}
}

void destroyDisplay(){
//	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(window);
}

}



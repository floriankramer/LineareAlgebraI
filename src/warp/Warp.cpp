/*
 * Warp.cpp
 *
 *  Created on: Nov 19, 2014
 *      Author: dwarf
 */

#include "Warp.h"

#define NO_SDL_GLEXT

#include <SDL2/SDL.h>
//#include <SDL2/SDL_opengl.h>
#include <vector>
#include <iostream>
#include <string>
#include <map>

#include <sys/time.h>

#include "RenderableSprite.h"
#include "TextureLoader.h"
#include "GraphicsOptions.h"
#include "DisplayHandler.h"
#include "Shader.h"
#include "glewWrapper.h"
#include "MatrixHandler.h"
#include "ShaderHandler.h"
#include "FBOHandler.h"
#include "LightHandler.h"
#include "FontRenderer.h"
#include "Renderer.h"
#include "os.h"
#include "OptionsHandler.h"
#include "CoordinateConverter.h"


#include "Game.h"
#include "main.h"

#include <string>

#ifdef windows
#include "windows.h"
#endif

namespace render{

void doRenderUpdate();
void loadNewTextures();
void syncRenderables();
void clearRenderables();
void cleanUp();
void createDefaultVao();
std::string glErrorToString(GLuint);


bool shouldRun = true;
SDL_Event event;
std::vector<Renderable *> *worldRenderables = new std::vector<Renderable *>();
std::vector<Renderable *> *hudRenderables = new std::vector<Renderable *>();

std::vector<VAOData*> vaos;

float cameraX = 0, cameraY = 0;
std::vector<PreTexture> preTextures;

std::map<std::string, Texture> textures;

Logger warpLogger("Warp");

GLint defaultVAOId = 0;

int fps[7] = {0, 0, 0, 0, 0, 0, 0};
int fpsIndex = 0;
bool displayFPS = false;

float updateFactor = 0.016666667;

#ifdef windows
DWORD WINAPI winRun(LPVOID lpParam){
	run(NULL);
	return 0;
}
#endif

void * run(void * atr){
	warpLogger.setName("render");
	warpLogger.log("Initializing SDL");
	//	SDL_Init(SDL_INIT_JOYSTICK);
	//	warpLogger.log("was init: " + std::to_string(SDL_WasInit(SDL_INIT_JOYSTICK)));

	try{
		createDisplay();
	}
	catch (const char * msg){
		warpLogger.log("EXCEPTION: " + std::string(msg));

		game::stop();
		if(physicsThread.isWaiting()){
			physicsThread.wakeUp();
		}
		SDL_Quit();
		return NULL;
	}

	checkForGLError("init");

	options::setPixelFormat(SDL_GetWindowPixelFormat(window));

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glEnable(GL_SCISSOR_TEST);

	glClearColor(0, 0, 0, 1);


	//set to 1 for vsync, 0 for max
	SDL_GL_SetSwapInterval(0);

	checkForGLError("basic GL setup");

	shader::init();
	checkForGLError("shader initialization");
	fbo::init();
	checkForGLError("vao initialization");
	font::init();
	checkForGLError("font initialization");
	light::init();

	createDefaultVao();

	warpLogger.log("setup finished");
	warpLogger.log("entering render loop");

#ifdef unix
	timespec timeStorage;
	long last = 0, current = 0;
	clock_gettime(CLOCK_MONOTONIC, &timeStorage);
	timespec pauseTime;
	pauseTime.tv_sec = 0;
	pauseTime.tv_nsec = 0;
#endif

	loadOptions();

	while(shouldRun){
#ifdef unix
		last = current;
		clock_gettime(CLOCK_MONOTONIC, &timeStorage);
		//gettimeofday(&timeStorage, NULL);
		current = 1000000 * timeStorage.tv_sec + timeStorage.tv_nsec / 1000; //time in us
		if(current - last != 0){
			fps[fpsIndex] = 1 / (static_cast<double>(current - last) / 1000000);
			fpsIndex = (fpsIndex + 1) % 7;
			updateFactor = (current - last) / 1000000.0;
		}

#endif

		syncRenderables();

		updateFullscreenState();

		if(displayFPS){
			setRenderTarget(HUD);
			drawString(options::getAspectRatio() - 0.3, 0.05, 0.05, "fps: " + std::to_string(static_cast<int>((fps[0] + fps[1] + fps[2] + fps[3] + fps[4] + fps[5] + fps[6]) / 7.0)));
			//drawString(getAspectRatio() - 0.3, 0.05, 0.05, "fps: " + std::to_string(fps[0]));
		}

		if(fbo::recreateFBOs){
			fbo::destroy();
			fbo::init();
			fbo::recreateFBOs = false;
		}
		if(light::isShouldRebuildShadowMap()){
			light::rebuildShadowMapInternal();
		}

		loadNewTextures();
		font::loadNewFonts();


		doRenderUpdate();

		checkForGLError("render loop");

#ifdef unix
		pauseTime.tv_sec = 0;
		//pauseTime.tv_nsec = 32000000;
		//pauseTime.tv_nsec = 800000 - ((clock() - current + 0.0) / CLOCKS_PER_SEC * 1000000000);
		//pauseTime.tv_nsec = 16000000 - ((clock() - current + 0.0) / CLOCKS_PER_SEC * 1000000000);
		//pauseTime.tv_nsec = 500000000 - ((clock() - current + 0.0) / CLOCKS_PER_SEC * 1000000000);
		pauseTime.tv_nsec = options::getUpdateTime() - (updateFactor * 1000000);
		while(nanosleep(&pauseTime, &pauseTime) && errno == EINTR);
#endif
#ifdef windows
		Sleep(options::getUpdateTime());
#endif
		SDL_GL_SwapWindow(window);


	}
	if(physicsThread.isWaiting()){
		physicsThread.wakeUp();
	}



	game::stop();

	cleanUp();

	SDL_Quit();
	return NULL;
}

#include "Mouse.h"

void doRenderUpdate(){
	/* DEBUG CODE for Collision handler line intersection testing
	 * fbo::setFBOTarget(fbo::DISPLAY);
	glClearColor(0, 0, 0, 1);
	glDisable(GL_TEXTURE_2D);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	int x, y;
	SDL_GetMouseState(&x, &y);
	float * m = pixelToScreenSpace(x, y);
	m[0] /= options::getAspectRatio();

	glColor4f(1, 1, 1, 1);

	glBegin(GL_TRIANGLES);
	glVertex2f(m[0] - 0.05, m[1] - 0.05);
	glVertex2f(m[0] + 0.05, m[1] - 0.05);
	glVertex2f(m[0] + 0.05, m[1] + 0.05);

	glVertex2f(m[0] - 0.05, m[1] - 0.05);
	glVertex2f(m[0] + 0.05, m[1] + 0.05);
	glVertex2f(m[0] - 0.05, m[1] + 0.05);
	glEnd();

	glBegin(GL_LINES);
	glVertex2f(-0.5, -0.3);
	glVertex2f(0.1, 0.4);

	glVertex2f(0.567, -.4124);
	glVertex2f(m[0], m[1]);
	glEnd();







	if(game::physics::linesIntersect(-0.5, 0.1, -0.3, 0.4, 0.567, m[0], -.4124, m[1])){
		glColor4f(0, 0, 1, 1);
		glBegin(GL_TRIANGLES);
		glVertex2f(m[0] - 0.015, m[1] - 0.015);
		glVertex2f(m[0] + 0.015, m[1] - 0.015);
		glVertex2f(m[0] + 0.015, m[1] + 0.015);

		glVertex2f(m[0] - 0.015, m[1] - 0.015);
		glVertex2f(m[0] + 0.015, m[1] + 0.015);
		glVertex2f(m[0] - 0.015, m[1] + 0.015);
		glEnd();
	}

	float *d = game::physics::getLineIntersection(-0.5, 0.1, -0.3, 0.4, 0.567, m[0], -.4124, m[1]);
	if(!isnan(d[0])){
		glColor4f(0, 0, 1, 1);
		glBegin(GL_TRIANGLES);
		glVertex2f(d[0] - 0.01, d[1] - 0.01);
		glVertex2f(d[0] + 0.01, d[1] - 0.01);
		glVertex2f(d[0] + 0.01, d[1] + 0.01);

		glVertex2f(d[0] - 0.01, d[1] - 0.01);
		glVertex2f(d[0] + 0.01, d[1] + 0.01);
		glVertex2f(d[0] - 0.01, d[1] + 0.01);
		glEnd();
	}


	if(game::physics::lineIntersectsRect(-0.5, 0.1, -0.3, 0.4, m[0] - 0.05, m[1] - 0.05, m[0] + 0.05, m[1] + 0.05)){
		glColor4f(0, 1, 0, 1);
		glBegin(GL_TRIANGLES);
		glVertex2f(m[0] - 0.01, m[1] - 0.01);
		glVertex2f(m[0] + 0.01, m[1] - 0.01);
		glVertex2f(m[0] + 0.01, m[1] + 0.01);

		glVertex2f(m[0] - 0.01, m[1] - 0.01);
		glVertex2f(m[0] + 0.01, m[1] + 0.01);
		glVertex2f(m[0] - 0.01, m[1] + 0.01);
		glEnd();
	}

	float *f = game::physics::getLineRectIntersection(-0.5, 0.1, -0.3, 0.4, m[0] - 0.05, m[1] - 0.05, m[0] + 0.05, m[1] + 0.05 );
	if(!isnan(f[0])){
		glColor4f(0, 1, 0, 1);
		glBegin(GL_TRIANGLES);
		glVertex2f(f[0] - 0.015, f[1] - 0.015);
		glVertex2f(f[0] + 0.015, f[1] - 0.015);
		glVertex2f(f[0] + 0.015, f[1] + 0.015);

		glVertex2f(f[0] - 0.015, f[1] - 0.015);
		glVertex2f(f[0] + 0.015, f[1] + 0.015);
		glVertex2f(f[0] - 0.015, f[1] + 0.015);
		glEnd();
	}

	//bottom
	f = game::physics::getLineIntersection(-0.5, 0.1, -0.3, 0.4, m[0] - 0.05, m[0] + 0.05, m[1] - 0.05, m[1] - 0.05);
	if(!isnan(f[0])){
		glColor4f(0.5, 0.5, 0, 1);
		glBegin(GL_TRIANGLES);
		glVertex2f(f[0] - 0.01, f[1] - 0.01);
		glVertex2f(f[0] + 0.01, f[1] - 0.01);
		glVertex2f(f[0] + 0.01, f[1] + 0.01);

		glVertex2f(f[0] - 0.01, f[1] - 0.01);
		glVertex2f(f[0] + 0.01, f[1] + 0.01);
		glVertex2f(f[0] - 0.01, f[1] + 0.01);
		glEnd();
	}

	//left
	f = game::physics::getLineIntersection(-0.5, 0.1, -0.3, 0.4, m[0] - 0.05, m[0] - 0.05, m[1] - 0.05, m[1] + 0.05);
	if(!isnan(f[0])){
		glColor4f(0.5, 0, 0.5, 1);
		glBegin(GL_TRIANGLES);
		glVertex2f(f[0] - 0.01, f[1] - 0.01);
		glVertex2f(f[0] + 0.01, f[1] - 0.01);
		glVertex2f(f[0] + 0.01, f[1] + 0.01);

		glVertex2f(f[0] - 0.01, f[1] - 0.01);
		glVertex2f(f[0] + 0.01, f[1] + 0.01);
		glVertex2f(f[0] - 0.01, f[1] + 0.01);
		glEnd();
	}

	//top
	f = game::physics::getLineIntersection(-0.5, 0.1, -0.3, 0.4, m[0] - 0.05, m[0] + 0.05, m[1] + 0.05, m[1] + 0.05);
	if(!isnan(f[0])){
		glColor4f(0, 0, 0.5, 1);
		glBegin(GL_TRIANGLES);
		glVertex2f(f[0] - 0.01, f[1] - 0.01);
		glVertex2f(f[0] + 0.01, f[1] - 0.01);
		glVertex2f(f[0] + 0.01, f[1] + 0.01);

		glVertex2f(f[0] - 0.01, f[1] - 0.01);
		glVertex2f(f[0] + 0.01, f[1] + 0.01);
		glVertex2f(f[0] - 0.01, f[1] + 0.01);
		glEnd();
	}

	//right
	f = game::physics::getLineIntersection(-0.5, 0.1, -0.3, 0.4, m[0] + 0.05, m[0] + 0.05, m[1] - 0.05, m[1] + 0.05);
	if(!isnan(f[0])){
		glColor4f(0.5, 0.2, 0.5, 1);
		glBegin(GL_TRIANGLES);
		glVertex2f(f[0] - 0.01, f[1] - 0.01);
		glVertex2f(f[0] + 0.01, f[1] - 0.01);
		glVertex2f(f[0] + 0.01, f[1] + 0.01);

		glVertex2f(f[0] - 0.01, f[1] - 0.01);
		glVertex2f(f[0] + 0.01, f[1] + 0.01);
		glVertex2f(f[0] - 0.01, f[1] + 0.01);
		glEnd();
	}


	delete[] m;

	return;*/

	if(light::isLightsEnabled())
		fbo::setFBOTarget(fbo::COLOR);
	else
		fbo::setFBOTarget(fbo::LIGHT);
	glClear(GL_COLOR_BUFFER_BIT);
	shader::setCurrentShader(shader::getMainShader());

	matrix::setupProjectionMatrix(options::getVirtualWidth() * 2 * options::getZoom(), options::getVirtualHeight() * 2 * options::getZoom());

	float cameraAlignedX = (int)(cameraX / pixelWidth) * pixelWidth;
	float cameraAlignedY = (int)(cameraY / pixelHeight) * pixelHeight;

	shader::uploadCameraLocation(cameraAlignedX, cameraAlignedY);

	matrix::setCameraPosition(cameraAlignedX, cameraAlignedY);

	for (Renderable *r : *worldRenderables){
		r->render();
	}

	if(light::isLightsEnabled()){
		fbo::setFBOTarget(fbo::LIGHT);

		shader::setCurrentShader(shader::getLightShader());

		glClear(GL_COLOR_BUFFER_BIT);

		glBlendFunc(GL_ONE, GL_ONE);

		glUniform1f(glGetUniformLocation(shader::getLightShaderId(), "screenWidth"), options::getVirtualWidth() / 2);
		glUniform1f(glGetUniformLocation(shader::getLightShaderId(), "screenHeight"), options::getVirtualHeight()/ 2);
		glUniform2f(glGetUniformLocation(shader::getLightShaderId(), "screenCenter"), cameraX, cameraY);

		glBindTexture(GL_TEXTURE_2D, fbo::getFBOTextureId(fbo::COLOR));

		int o = 0;
		int i;
		for(i = 0; i <= (light::getLightNumber() - 1) / options::getMaxLights(); i++){
			o += render::light::uploadLightData(shader::getLightShader(), o);


			glBindVertexArray(defaultVAOId);
			//    		glEnableVertexAttribArray(0);
			//    		glEnableVertexAttribArray(1);

			glDrawArrays(GL_TRIANGLES, 0, 6);

			//    		glDisableVertexAttribArray(0);
			//    		glDisableVertexAttribArray(1);
			glBindVertexArray(0);
		}

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	}
	//render the HUD
	fbo::setFBOTarget(fbo::LIGHT);
	//	glClear(GL_COLOR_BUFFER_BIT);
	shader::setCurrentShader(shader::getMainShader());
	matrix::setCameraPosition(0, 0);
	matrix::loadIdentity();
	matrix::setupProjectionMatrix(options::getAspectRatio() * 2, 2);
	for(Renderable *r : *hudRenderables){
		r->render();
	}

	fbo::setFBOTarget(fbo::DISPLAY);
	shader::setCurrentShader(shader::getPostProcessingShader());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//render the COLOR fbo onto the display
	glBindTexture(GL_TEXTURE_2D, fbo::getFBOTextureId(fbo::LIGHT));
	glBindVertexArray(defaultVAOId);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//render the HUD fbo onto the display
	//	glBindTexture(GL_TEXTURE_2D, fbo::getFBOTextureId(fbo::HUD));
	//	glBindVertexArray(getDefaultVaoId());
	//	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	light::debug();

}

void syncRenderables(){
	if(physicsThread.isWaiting()){
		SDL_PumpEvents();
		//		warpLogger.log("waking up physics");
		physicsThread.wakeUp();
		//		warpLogger.log("falling asleep");
		renderThread.wait();
		//		warpLogger.log("awakened");

	}
	else{
		warpLogger.log("physics thread not yet waiting!");
	}

}

void clearRenderables(){
	for (Renderable *r : *worldRenderables){
		delete r;
	}
	for (Renderable *r : *hudRenderables){
		delete r;
	}
	worldRenderables->clear();
	hudRenderables->clear();
}

void resetRenderables(){
	for (Renderable *r : *worldRenderables){
		if(!r->getSustain())
			delete r;
	}
	for (Renderable *r : *hudRenderables){
		if(!r->getSustain())
			delete r;
	}
	worldRenderables->clear();
	hudRenderables->clear();
}

void cleanUp(){
	shader::destroy();
	fbo::destroy();

	for(auto i : textures){
		Texture t = i.second;
		glDeleteTextures(1, &t.id);
	}

	for(VAOData* v : vaos){
		glDeleteBuffers(1, &v->vboId);
		glDeleteVertexArrays(1, &v->id);
		delete v;
	}

	clearRenderables();

	delete worldRenderables;
	delete hudRenderables;

	destroyDisplay();

}

void addTextureToLoad(PreTexture t){
	lockPreTextures();
	preTextures.push_back(t);
	unlockPreTextures();
}

void loadNewTextures(){
	lockPreTextures();
	for(unsigned int i = 0; i < preTextures.size(); i++){

		PreTexture *t = &preTextures.at(i);
		warpLogger.log(std::string("loading texture: ") + t->name + " with size: " + std::to_string(t->textureWidth) + " " + std::to_string(t->textureHeight));
		int mode = GL_RGB;
		if(t->data->getDepth() == 4){
			mode = GL_RGBA;
		}
		//		if(t.data->format->BytesPerPixel == 4){
		//			mode = GL_RGBA;
		//		}

		unsigned int id;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);
		//glTexImage2D(GL_TEXTURE_2D, 0, mode, t.data->w, t.data->h, 0, mode, GL_UNSIGNED_BYTE, t.data->pixels);
		glTexImage2D(GL_TEXTURE_2D, 0, mode, t->data->getWidth(), t->data->getHeight(), 0, mode, GL_UNSIGNED_BYTE, t->data->getPixel());

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //GL_NEAREST
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //GL_NEAREST

		glBindTexture(GL_TEXTURE_2D, 0);

		//	free(buffer);

		delete t->data;

		//SDL_FreeSurface(t.data);
		warpLogger.log("loaded texture");
		Texture texture;
		texture.id = id;
		texture.vao = createVAO((t->srcWidth + 0.0) / t->textureWidth, (t->srcHeight + 0.0) / t->textureHeight);
		textures[t->name] = texture;
	}

	preTextures.clear();
	unlockPreTextures();
}

void setCameraPosition(float x, float y){
	cameraX = x;
	cameraY = y;
}

void stop(){
	shouldRun = false;
}

Texture* getTexture(std::string name){
	if(textures.find(name) != textures.end()){
		return &textures[name];
	}
	return NULL;
}

void addRenderable(Renderable *r, RenderTarget t){
	switch(t){
	case WORLD:
		worldRenderables->push_back(r);
		break;
	case HUD:
		hudRenderables->push_back(r);
		break;
	default:
		worldRenderables->push_back(r);
		break;
	}
}

void checkForGLError(std::string location){
	int errorValue = glGetError(); //check for GL errors
	if (errorValue != GL_NO_ERROR) {
		warpLogger.log(std::string("GL Error at ") + location + ": " + glErrorToString(errorValue));
	}
}

void setScissorArea(float x, float y, float width, float height){
	//	int *f1 = screenToPixelSpace(x, y); //TODO check y orientation returned vs y orientation used by glScissor (up / down)
	//	int *f2 = screenToPixelSpace(x + width, y + height);

	//glScissor(f1[0], f1[1], f2[0] - f1[0], f2[1] - f1[1] + 10);
	//	glScissor(0, 0, 200, 200); TODO save with renderables to be run from the right thread

	//	delete[] f1;
	//	delete[] f2;
}

GLint getDefaultVaoId(){
	return defaultVAOId;
}

void createDefaultVao(){
	VAOData* t = createVAO(1, 1);
	defaultVAOId = t->id;
}

VAOData* createVAO(float maxU, float maxV){
	return createVAO(0, 0, maxU, maxV);
}

VAOData* createVAO(float minU, float minV, float maxU, float maxV){
	for(VAOData* v : vaos){
		if(v->maxU == maxU && v->maxV == maxV && v->minU == minU && v->minV == minV){
			return v;
		}
	}

	VAOData *vao = new VAOData();

	GLfloat vertexBufferData[] = {
			1, 1, 1, maxU, maxV,
			-1, -1, 1, minU, minV,
			1, -1, 1, maxU, minV,
			1, 1, 1, maxU, maxV,
			-1, 1, 1, minU, maxV,
			-1, -1, 1, minU, minV
	};

	vao->minU = minU;
	vao->minV = minV;
	vao->maxU = maxU;
	vao->maxV = maxV;
	vao->width = maxU - minU;
	vao->height = maxV - minV;

	glGenVertexArrays(1, &vao->id);
	glBindVertexArray(vao->id);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &vao->vboId);
	glBindBuffer(GL_ARRAY_BUFFER, vao->vboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(vertexBufferData)[0], 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(vertexBufferData)[0], (const GLvoid *)(3 * sizeof(vertexBufferData)[0]));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	vaos.push_back(vao);

	checkForGLError("vao creation");

	return vao;
}


std::string glErrorToString(GLuint error){
	std::string s;
	if(error == GL_INVALID_ENUM){
		s = "Invalid Enum";
	}
	else if(error == GL_INVALID_VALUE){
		s = "Invalid Value";
	}
	else if(error == GL_INVALID_OPERATION){
		s = "Invalid Operation";
	}
	else if(error == GL_STACK_OVERFLOW){
		s = "Stack Overflow";
	}
	else if(error == GL_STACK_UNDERFLOW){
		s = "Stack Underflow";
	}
	else if(error == GL_OUT_OF_MEMORY){
		s = "Out of Memory";
	}
	else if(error == GL_TABLE_TOO_LARGE){
		s = "Table to Large";
	}
	return s;
}

float getCameraX(){
	return cameraX;
}

float getCameraY(){
	return cameraY;
}

void setShowFPSCounter(bool b){
	displayFPS = b;
}

float getUpdateFactor(){
	return updateFactor;
}
}

#ifndef WARP_H
#define WARP_H

#include "Vec2f.h"
#include "TextureLoader.h"
#include "Logger.h"
#include "Renderable.h"
#include "Renderer.h"
#include "VAOData.h"
#include "Shader.h"
#include "os.h"

namespace warp{

	const int WARP_TARGET_SCENE = 0;
	const int WARP_TARGET_UI = 1;

	extern Logger warpLogger;

	struct Texture{
		unsigned int id;
		VAOData *vao;
	};

	void init(bool multiThreaded);
	void update();
	void cleanup();	
	void registerRenderable(int renderable, int renderTarget);
	void setCameraPosition(float posX, float posY);
	void stop();
	Vec2f getGlobalCoordinatesOfMouseEvent(int eventX, int eventY);
	void addTextureToLoad(PreTexture t);
	Texture* getTexture(std::string);
	void addRenderable(Renderable *, RenderTarget t);
	void checkForGLError(std::string);
	GLint getDefaultVaoId();
	GLuint getCircleVaoId();
	float* screenToWorldSpace(float, float);
	float *worldToScreenSpace(float x, float y);
	VAOData* createVAO(float minU, float minV, float maxU, float maxV);
	VAOData* createVAO(float maxU, float maxV);
	void resetRenderables();

	float getUpdateFactor();

	float getCameraX();
	float getCameraY();

	int getFPS();
	void setShowFPSCounter(bool b);

#ifdef windows
	DWORD WINAPI winRun(LPVOID lpParam);
#endif
}

#endif



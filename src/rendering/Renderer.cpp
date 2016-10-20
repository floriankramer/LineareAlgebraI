// Copyright 2016 Florian Kramer

#include "Renderer.h"

#include <iostream>
#include <cmath>
#include <map>
#include <vector>

#include "Warp.h"
#include "FBOHandler.h"

#include "Renderable.h"
#include "RenderableSprite.h"
#include "RenderableLine.h"
#include "RenderableRect.h"
#include "RenderableString.h"
#include "RenderableCurve.h"
#include "RenderableScissor.h"
#include "RenderableCircle.h"
#include "RenderableStatic.h"
#include "RenderableFbo.h"


namespace warp{

	std::map<int, StaticRenderable> staticRenderables;
	int staticRenderableIdCounter = 0;

	//Mutex renderablesMutex;
	RenderTarget target = WORLD;

	void drawRenderable(Renderable *r){
		addRenderable(r, target);
	}

	void drawSprite(float x, float y, float width, float height, float rotation, std::string texture){
		addRenderable(new RenderableSprite(x, y, width, height, rotation, texture), target);
	}	
  
  void drawSprite(float x, float y, float width, float height, float rotation, unsigned int textureId){
		addRenderable(new RenderableSprite(x, y, width, height, rotation, textureId), target);
	}

	void drawLine(float x, float y, float x2, float y2, float thickness, Color c){
		addRenderable(new RenderableLine(x, y, x2, y2, thickness, c), target);
	}

	void drawRect(float x, float y, float width, float height, float rot, Color c){
		addRenderable(new RenderableRect(x, y, width, height, rot, c), target);
	}

	void drawCurve(float x1, float y1, float x2, float y2, float dx1, float dy1, float dx2, float dy2, float thickness, Color c){
		addRenderable(new RenderableCurve(x1, x2, y1, y2, dx1, dx2, dy1, dy2, thickness, c), target);
	}

	void drawCircle(float x, float y, float radius, Color c){
		addRenderable(new RenderableCircle(x, y, radius, c), target);
	}

	void drawRectOutline(float x, float y, float width, float height, float rot, float thickness, Color c){
		float halfwidth = width / 2;
		float halfheight = height / 2;
		float halfthickness = thickness / 2;
		drawRect(x - halfwidth + halfthickness, y, thickness, height, 0, c);
		drawRect(x, y + halfheight - halfthickness, width - 2 * thickness, thickness, 0, c);
		drawRect(x + halfwidth - halfthickness, y, thickness, height, 0, c);
		drawRect(x, y - halfheight + halfthickness, width - 2 * thickness, thickness, 0, c);
	}

	void drawCircleOutline(float x, float y, float radius, Color c){
		float angle = 0;
		float angleStep = 3.1415 * 2 / 16;
		for(int i = 0; i < 16; i++){
			drawLine(x + sin(angle) * radius, y + cos(angle) * radius, x + sin(angle + angleStep) * radius, y + cos(angle + angleStep) * radius, 0.1, c);
			angle += angleStep;
		}
	}

	void drawString(float x, float y, float lineheight, std::string string, Color c){
		addRenderable(new RenderableString(x, y, lineheight, string, c), target);
	}
    // pushes the given fbo onto the fbo stack, making it the current fbo target
  void setCurrentFbo(unsigned int fbo) {
    addRenderable(new RenderableFbo(fbo, false), target);
  }


  void createFboForId(unsigned int id, unsigned int width, unsigned int height) {
    addRenderable(new RenderableFbo(id, width, height), target);
  }

  void clearCurrentFbo() {
    addRenderable(new RenderableFbo(0, true), target);
  }

	void setRenderTarget(RenderTarget t){
		target = t;
	}

	void setScissorArea(float x, float y, float width, float height){
		addRenderable(new RenderableScissor(x, y, width, height), target);
	}

	RenderableSprite *createRenderableSprite(float x, float y, float width, float height, float rotation, std::string texture){
		return new RenderableSprite(x, y, width, height, rotation, texture);
	}

	RenderableSprite *createRenderableSprite(float x, float y, float width, float height, float rotation, Texture *texture){
		return new RenderableSprite(x, y, width, height, rotation, texture);
	}


	int createStaticLineRenderable(float *vertices, int numVertecies, float linewidth, Color c){
		if(numVertecies % 2 != 0){
			return -1;
		}

		StaticRenderable sr;
		sr.color = c;
		sr.vertexCount = numVertecies * 3;
		sr.drawType = GL_TRIANGLES;

		linewidth /= 2;

		int numData = numVertecies * 9;

		GLfloat *vertexBufferData = new GLfloat[numData]; //TODO reuse vertex data

		float cv[2]; //cross vector
		for(int i = 0; i < numVertecies / 2; i++){ //for every line
			float dx = vertices[i * 4 + 2] - vertices[i * 4];
			float dy = vertices[i * 4 + 3] - vertices[i * 4 + 1];
			float l = std::hypot(dx, dy);
			cv[0] = dy / l * linewidth;
			cv[1] = -dx / l * linewidth;

			//first triangle
			vertexBufferData[i * 18 + 0] = vertices[i * 4 + 0]  + cv[0]; 
			vertexBufferData[i * 18 + 1] = vertices[i * 4 + 1]  + cv[1]; 
			vertexBufferData[i * 18 + 2] = 1;

			vertexBufferData[i * 18 + 3] = vertices[i * 4 + 0]  - cv[0]; 
			vertexBufferData[i * 18 + 4] = vertices[i * 4 + 1]  - cv[1]; 
			vertexBufferData[i * 18 + 5] = 1; 

			vertexBufferData[i * 18 + 6] = vertices[i * 4 + 2]  + cv[0]; 
			vertexBufferData[i * 18 + 7] = vertices[i * 4 + 3]  + cv[1]; 
			vertexBufferData[i * 18 + 8] = 1; 


			//second triangle
			vertexBufferData[i * 18 + 9] = vertices[i * 4 + 0]  - cv[0]; 
			vertexBufferData[i * 18 + 10] = vertices[i * 4 + 1]  - cv[1]; 
			vertexBufferData[i * 18 + 11] = 1;

			vertexBufferData[i * 18 + 12] = vertices[i * 4 + 2]  + cv[0]; 
			vertexBufferData[i * 18 + 13] = vertices[i * 4 + 3]  + cv[1]; 
			vertexBufferData[i * 18 + 14] = 1; 

			vertexBufferData[i * 18 + 15] = vertices[i * 4 + 2]  - cv[0]; 
			vertexBufferData[i * 18 + 16] = vertices[i * 4 + 3]  - cv[1]; 
			vertexBufferData[i * 18 + 17] = 1; 
		}

		glGenVertexArrays(1, &sr.vaoId);
		glBindVertexArray(sr.vaoId);

		glEnableVertexAttribArray(0);
//		glEnableVertexAttribArray(1);

		glGenBuffers(1, &sr.vboId);
		glBindBuffer(GL_ARRAY_BUFFER, sr.vboId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * (numData), vertexBufferData, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
//		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (const GLvoid *)(3 * sizeof(GLfloat)));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		delete [] vertexBufferData;
		staticRenderables[staticRenderableIdCounter] = sr;
		staticRenderableIdCounter ++;
		return staticRenderableIdCounter - 1;
	}


  int createStaticRectRenderable(float *vertices, int numVertecies, Color c) {

		if(numVertecies % 4 != 0){
			return -1;
		}

		StaticRenderable sr;
		sr.color = c;
		sr.vertexCount = numVertecies + numVertecies / 2;
		sr.drawType = GL_TRIANGLES;	

		int numData = numVertecies * 4 + numVertecies / 2;

		GLfloat *vertexBufferData = new GLfloat[numData]; //TODO reuse vertex data
	
		for(int i = 0; i < numVertecies / 4; i++){ //for every rectangle
			//first triangle
			vertexBufferData[i * 18 + 0] = vertices[i * 8 + 0]; 
			vertexBufferData[i * 18 + 1] = vertices[i * 8 + 1]; 
			vertexBufferData[i * 18 + 2] = 1;

			vertexBufferData[i * 18 + 3] = vertices[i * 8 + 2]; 
			vertexBufferData[i * 18 + 4] = vertices[i * 8 + 3]; 
			vertexBufferData[i * 18 + 5] = 1; 

			vertexBufferData[i * 18 + 6] = vertices[i * 8 + 4]; 
			vertexBufferData[i * 18 + 7] = vertices[i * 8 + 5]; 
			vertexBufferData[i * 18 + 8] = 1; 


			//second triangle
			vertexBufferData[i * 18 + 9] = vertices[i * 8 + 4]; 
			vertexBufferData[i * 18 + 10] = vertices[i * 8 + 5]; 
			vertexBufferData[i * 18 + 11] = 1;

			vertexBufferData[i * 18 + 12] = vertices[i * 8 + 6]; 
			vertexBufferData[i * 18 + 13] = vertices[i * 8 + 7]; 
			vertexBufferData[i * 18 + 14] = 1; 

			vertexBufferData[i * 18 + 15] = vertices[i * 8 + 0]; 
			vertexBufferData[i * 18 + 16] = vertices[i * 8 + 1]; 
			vertexBufferData[i * 18 + 17] = 1; 
		}

		glGenVertexArrays(1, &sr.vaoId);
		glBindVertexArray(sr.vaoId);

		glEnableVertexAttribArray(0);
//		glEnableVertexAttribArray(1);

		glGenBuffers(1, &sr.vboId);
		glBindBuffer(GL_ARRAY_BUFFER, sr.vboId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * (numData), vertexBufferData, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
//		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (const GLvoid *)(3 * sizeof(GLfloat)));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		delete [] vertexBufferData;
		staticRenderables[staticRenderableIdCounter] = sr;
		staticRenderableIdCounter ++;
		return staticRenderableIdCounter - 1;
  }

	void deleteStaticRenderable(int id){
		if(staticRenderables.find(id) != staticRenderables.end()){
			StaticRenderable *sr = &staticRenderables[id];
			glDeleteBuffers(1, &sr->vboId);
			glDeleteVertexArrays(1, &sr->vaoId);
			staticRenderables.erase(staticRenderables.find(id));
		}

	}

	void drawStaticRenderable(int id){
		addRenderable(new RenderableStatic(&staticRenderables[id]), target);
	}
}



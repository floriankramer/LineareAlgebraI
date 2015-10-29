/*
 * FBOHandler.cpp
 *
 *  Created on: Dec 19, 2014
 *      Author: dwarf
 */

#include "FBOHandler.h"
#include "GraphicsOptions.h"
#include "Warp.h"

namespace render{
namespace fbo{

bool recreateFBOs = false;

struct FBO{
	GLuint id;
	GLuint texture;
};

FBO colorFBO, lightFBO, hudFBO;

void createFBO(FBO*);
void deleteFBO(FBO*);
void checkForFBOCreationError();

int fboWidth = 1920, fboHeight = 1080;

void init(){
	fboWidth = options::getFBOWidth();
	fboHeight = options::getFBOHeight();
	createFBO(&colorFBO);
	createFBO(&lightFBO);
	createFBO(&hudFBO);
}
void destroy(){
	deleteFBO(&colorFBO);
	deleteFBO(&lightFBO);
	deleteFBO(&hudFBO);
}

void createFBO(FBO* f){
	glGenFramebuffers(1, &f->id);
	glBindFramebuffer(GL_FRAMEBUFFER, f->id);

	glGenTextures(1, &f->texture);
	glBindTexture(GL_TEXTURE_2D, f->texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//ByteBuffer b = ByteBuffer.allocateDirect(Display.getWidth() * Display.getHeight() * 3);
	unsigned char *buffer = (unsigned char *)malloc(fboWidth * fboHeight * 3);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fboWidth, fboHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, f->texture, 0);

	checkForFBOCreationError();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	free(buffer);
}

void deleteFBO(FBO *f){
	glDeleteTextures(1, &f->texture);
	glDeleteFramebuffers(1, &f->id);
}


void checkForFBOCreationError(){
	int framebuffer = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	switch ( framebuffer ) {
	case GL_FRAMEBUFFER_COMPLETE:
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		warpLogger.log("Error while creating FBO: Incomplete Attachment");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		warpLogger.log("Error while creating FBO: Missing Attachment");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		warpLogger.log("Error while creating FBO: Incomplete Draw Buffer");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		warpLogger.log("Error while creating FBO: Incomplete Read Buffer");
		break;
	default:
		warpLogger.log("Error while creating FBO: Unknown Reason");
		break;
	}
}

void setFBOTarget(FBOTarget t){
	bool foundFbo = true;
	switch(t){
	case COLOR:
		glBindFramebuffer(GL_FRAMEBUFFER, colorFBO.id);
		break;
	case LIGHT:
		glBindFramebuffer(GL_FRAMEBUFFER, lightFBO.id);
		break;
	case HUD:
		glBindFramebuffer(GL_FRAMEBUFFER, hudFBO.id);
		break;
	case DISPLAY:
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		foundFbo = false;
		glViewport(0, 0, options::getDisplayWidth(), options::getDisplayHeight());
		break;
	default:
		foundFbo = false;
		break;
	}
	if(foundFbo)
		glViewport(0, 0, fboWidth, fboHeight);
}

GLuint getFBOTextureId(FBOTarget t){
	switch (t){
	case COLOR:
		return colorFBO.texture;
	case LIGHT:
			return lightFBO.texture;
	case HUD:
			return hudFBO.texture;
	default:
		return 0;
	}
}

}
}



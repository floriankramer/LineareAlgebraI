/*
 * ShaderHandler.h
 *
 *  Created on: Dec 19, 2014
 *      Author: dwarf
 */

#ifndef SHADERHANDLER_H_
#define SHADERHANDLER_H_

#include "Shader.h"
#include "Renderer.h"

namespace render{
namespace shader{

void init();
void destroy();
Shader* getMainShader();
int getMainShaderId();
Shader* getPostProcessingShader();
int getPostProcessingShaderId();
Shader* getLightShader();
int getLightShaderId();
Shader* getFontShader();
int getFontShaderId();
Shader *getCurrentShader();
void setCurrentShader(Shader *);


void setShaderUseTexture(bool);
void setShaderColor(Color c);
void uploadCameraLocation(float x, float y);

}
}



#endif /* SHADERHANDLER_H_ */

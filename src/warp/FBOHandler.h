#ifndef FBOHANDLER_H_
#define FBOHANDLER_H_

#include "glewWrapper.h"

namespace render{
namespace fbo{

enum FBOTarget{COLOR, LIGHT, HUD, DISPLAY};

void init();
void destroy();
void setFBOTarget(FBOTarget);
GLuint getFBOTextureId(FBOTarget);
extern bool recreateFBOs;

}
}

#endif

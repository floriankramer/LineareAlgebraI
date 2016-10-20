#ifndef FBOHANDLER_H_
#define FBOHANDLER_H_

#include "glewWrapper.h"

namespace warp{
	namespace fbo{

		void init();
		void destroy();
		void setFbo(unsigned int id); 
    unsigned int getCurrentFboId();
		GLuint getFboTextureId(unsigned int id);
		extern bool recreateFbos;
		GLuint getFBOId(unsigned int id);

    // threadsafe method to obtain an fbo id
    unsigned int obtainFboId();

    unsigned int createFbo(unsigned int width, unsigned int height);
    void createFbo(unsigned int id, unsigned int width, unsigned int height);
    void deleteFbo(unsigned int id);

    unsigned int getDefaultFboWidth();
    unsigned int getDefaultFboHeight();

    unsigned int getColorFboId();
    unsigned int getLightFboId();
    unsigned int getHudFboId();
	}
}

#endif

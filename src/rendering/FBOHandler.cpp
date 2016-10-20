// Copyright 2016 Florian Kramer

#include "FBOHandler.h"

#include <unordered_map>
#include <mutex>

#include "GraphicsOptions.h"
#include "Warp.h"

namespace warp{
  namespace fbo{

    bool recreateFbos = false;

    struct Fbo{
      GLuint id;
      GLuint texture;
      unsigned int width, height;
    };

    std::unordered_map<unsigned int, Fbo> fbos;

    // used to assign incremental ids
    unsigned int lastFboId = 1;
    std::mutex fboIdMutex;


    unsigned int colorFbo, lightFbo, hudFbo;
    unsigned int currentFbo= 0;

    void checkForFboCreationError();

    int fboWidth = 1920, fboHeight = 1080;

    void init() {
      fboWidth = options::getFBOWidth();
      fboHeight = options::getFBOHeight();
      colorFbo = createFbo(fboWidth, fboHeight);
      lightFbo = createFbo(fboWidth, fboHeight);
      hudFbo = createFbo(fboWidth, fboHeight);
    }
    void destroy() {
      deleteFbo(colorFbo);
      deleteFbo(lightFbo);
      deleteFbo(hudFbo);
    }

    unsigned int getCurrentFboId() {
      return currentFbo;
    }


		GLuint getFboTextureId(unsigned int id) {
      auto a = fbos.find(id);
      if (a != fbos.end()) {
        return a->second.texture;
      }
      return 0;
    }

    unsigned int obtainFboId() {
      unsigned int id;
      fboIdMutex.lock();
      id = lastFboId;
      lastFboId ++;
      fboIdMutex.unlock();
      return id; 
    }

    unsigned int createFbo(unsigned int width, unsigned int height) {
      unsigned int id = obtainFboId();
      createFbo(id, width, height);
      return id;
    }


    void createFbo(unsigned int id, unsigned int width, unsigned int height) {
      Fbo fbo;
      fbo.width = width;
      fbo.height = height;
      glGenFramebuffers(1, &fbo.id);
      glBindFramebuffer(GL_FRAMEBUFFER, fbo.id);

      glGenTextures(1, &fbo.texture);
      glBindTexture(GL_TEXTURE_2D, fbo.texture);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
      unsigned char *buffer = (unsigned char *)malloc(fbo.width * fbo.height * 3);

      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fbo.width, fbo.height, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);
      glBindTexture(GL_TEXTURE_2D, 0);

      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo.texture, 0);

      checkForFboCreationError();

      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      free(buffer);

      fbos[id] = fbo; 
    }

    void deleteFbo(unsigned int id) {
      auto a = fbos.find(id);
      if (a != fbos.end()) {
        Fbo fbo = a->second;
        fbos.erase(a);
        glDeleteTextures(1, &fbo.texture);
        glDeleteFramebuffers(1, &fbo.id);
      }
    }


    void checkForFboCreationError() {
      int framebuffer = glCheckFramebufferStatus(GL_FRAMEBUFFER);
      switch ( framebuffer ) {
        case GL_FRAMEBUFFER_COMPLETE:
          break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
          warpLogger.log("Error while creating Fbo: Incomplete Attachment");
          break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
          warpLogger.log("Error while creating Fbo: Missing Attachment");
          break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
          warpLogger.log("Error while creating Fbo: Incomplete Draw Buffer");
          break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
          warpLogger.log("Error while creating Fbo: Incomplete Read Buffer");
          break;
        default:
          warpLogger.log("Error while creating Fbo: Unknown Reason");
          break;
      }
    }

    void setFbo(unsigned int id) {
      auto a = fbos.find(id);

      if (id == 0) {
        // set current fbo to display
        glBindFramebuffer(GL_FRAMEBUFFER,0); 
        glViewport(0, 0, options::getDisplayWidth(), options::getDisplayHeight());
      } else if (a != fbos.end()) {
        glBindFramebuffer(GL_FRAMEBUFFER, a->second.id);
        glViewport(0, 0, a->second.width, a->second.height);
      } else {
        warpLogger.err("Fbo %d does not exist\n", id);
      }
    }

    unsigned int getDefaultFboWidth() {
      return fboWidth;
    }

    unsigned int getDefaultFboHeight() {
      return fboHeight;
    }

    unsigned int getColorFboId() {
      return colorFbo;
    }

    unsigned int getLightFboId() {
      return lightFbo;
    }

    unsigned int getHudFboId() {
      return hudFbo;
    }
  }
}

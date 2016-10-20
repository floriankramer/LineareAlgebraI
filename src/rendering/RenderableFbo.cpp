// Copyright 2016 Florian Kramer

#include "RenderableFbo.h"
#include "FBOHandler.h"

namespace warp {

  unsigned int lastFboId = -1;

  RenderableFbo::RenderableFbo(unsigned int id, bool reset) :
    id(id),
    reset(reset),
    create(false),
    width(0),
    height(0) {
    }
  RenderableFbo::RenderableFbo(unsigned int id, unsigned int width, unsigned int height) :
    id(id),
    reset(false),
    create(true),
    width(width),
    height(height) {
    }

  void RenderableFbo::render() {
    if (create) {
      fbo::createFbo(id, width, height);
    } else {
      if (reset) {
        glEnable(GL_BLEND);
        if (lastFboId != static_cast<unsigned int>(-1)) {
          fbo::setFbo(lastFboId);
        }
      } else {
        glDisable(GL_BLEND);
        lastFboId = fbo::getCurrentFboId();
        fbo::setFbo(id);
      }
    }
  }

}

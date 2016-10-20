// Copyright 2016 Florian Kramer

#ifndef WARP_RENDERABLE_FBO_H_
#define WARP_RENDERABLE_FBO_H_

#include "Renderable.h"

namespace warp {
  class RenderableFbo : public Renderable {
   public:
      RenderableFbo(unsigned int id, bool reset);
      RenderableFbo(unsigned int id, unsigned int width, unsigned int height);
      void render();

   private:
      unsigned int id;
      bool reset;
      bool create;
      unsigned int width;
      unsigned int height;
  };
}

#endif  //WARP_RENDERABLE_FBO_H_

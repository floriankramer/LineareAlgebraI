/*
 * RenderableSprite.h
 *
 *  Created on: Dec 2, 2014
 *      Author: dwarf
 */

#ifndef RENDERABLESPRITE_H_
#define RENDERABLESPRITE_H_

#include "Renderable.h"
#include "Warp.h"
#include <string>

namespace warp{
  class RenderableSprite: public Renderable {
   public:
      RenderableSprite(float x, float y, float width, float height, float rot, std::string texture);
      RenderableSprite(float x, float y, float width, float height, float rot, unsigned int textureId);
      RenderableSprite(float x, float y, float width, float height, float rot, Texture *texture);
      virtual ~RenderableSprite();

      void render();

      void setTransform(float x, float y, float width, float height, float rot);
      void setTransform(float x, float y, float width, float height);

      RenderableSprite *clone();
      RenderableSprite *cloneWithTransforms(float x, float y, float width, float height, float rot);

   private:
      float width, height, rot;
      std::string textureName;
      Texture *texture;
      unsigned int textureId;
  };
}

#endif /* RENDERABLESPRITE_H_ */

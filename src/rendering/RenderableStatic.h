/*
 * RenderableStatic.h
 *
 *  Created on: 2016-04-01
 *      Author: dwarf
 */

#ifndef RENDERABLESTATIC_H_
#define RENDERABLESTATIC_H_

#include "Renderable.h"

namespace warp {

	class RenderableStatic: public Renderable {
		public:
			RenderableStatic(StaticRenderable *sr);
			virtual ~RenderableStatic();
			void render();
		private:
			StaticRenderable *sr; 
	};

} 

#endif 

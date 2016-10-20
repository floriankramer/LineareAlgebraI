/*
 * LightHandler.h
 *
 *  Created on: Dec 27, 2014
 *      Author: dwarf
 */

#ifndef LIGHTHANDLER_H_
#define LIGHTHANDLER_H_

#include "glewWrapper.h"
#include "Shader.h"

namespace warp{
	namespace light{

		struct Light{
			float x, y;
			float strength;

			GLint uniformLocPos = -1;
			GLint uniformLocStrength;
			GLint uniformLocId;
		};


		int addLight(float x, float y, float strength);
		int getLightNumber();
		void setLightPosition(int, float, float);

		// Upload the currently rendered subset of lights
		// to the graphics card.
		// Returns the new offset into the light vector,
		// that can then be passed to the function again after a render call,
		// to upload the next set of lights until offset == getLightNumber(); 
		int uploadLightData(Shader *, int offset);

		void setLightEnabled(bool b);
		bool isLightsEnabled();
		Light *getLight(int i);
		void clearLights();
		void removeLight(int);
		void render(float updateFactor);
		void init();
		void rebuildShadowMapInternal();
		void rebuildShadowMap();
		bool isShouldRebuildShadowMap();
		int getNumShadows();

		// renders debug info for the lights
		void debugGame();
		void debug();

		extern const float lightFalloffFactor;

	}
}



#endif /* LIGHTHANDLER_H_ */

/*
 * FontRenderer.h
 *
 *  Created on: Dec 30, 2014
 *      Author: dwarf
 */

#ifndef FONTRENDERER_H_
#define FONTRENDERER_H_

#include "glewWrapper.h"
#include "VAOData.h"
#include <string>

namespace warp{
	namespace font{

		void init();
		std::string loadFont(std::string);
		GLuint getFontTexture();
		GLuint getVaoId(char c);
		VAOData *getVao(char c);
		float getOffset(char c);

		void addFontToLoad(std::string s);
		void loadNewFonts();

	}
}


#endif /* FONTRENDERER_H_ */

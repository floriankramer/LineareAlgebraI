
#ifndef ANIMATIONLAODER_H_
#define ANIMATIONLAODER_H_
#include <string>
#include "Character.h"

namespace warp {
	namespace animation {

		// loads the given SCML (Spriter Animation) file, making all
		// the characters contained available to the getCharacter call
		//
		// the texturePath describes the base path in which to look for
		// the characters textures
		void loadSCMLFile(std::string src, std::string texturePath);


		// returns a character previously loaded from a SCML file
		// using the loadSCMLFile method
		Character *getCharacter(std::string name);

	}
}
#endif

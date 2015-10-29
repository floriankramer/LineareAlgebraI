
#ifndef ANIMATIONLAODER_H_
#define ANIMATIONLAODER_H_
#include <string>
#include "Character.h"

namespace render {
namespace animation {

void loadSCMLFile(std::string src, std::string texturePath);
Character *getCharacter(std::string name);

}
}
#endif

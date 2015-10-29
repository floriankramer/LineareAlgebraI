/*
 * Shader.h
 *
 *  Created on: Dec 11, 2014
 *      Author: dwarf
 */

#ifndef SHADER_H_
#define SHADER_H_

//#include <GL/glew.h>
#include "glewWrapper.h"
#include <string>

namespace render {

class Shader {
public:
	Shader(std::string);
	virtual ~Shader();
	GLuint getProgramId();
	GLuint getProjectionMatrixLocation();
	GLuint getModelViewMatrixLocation();
private:
	GLuint programId;
	GLuint fragmentId;
	GLuint vertexId;
	GLuint projectionMatrixLocation, modelViewMatrixLocation;

};

} /* namespace render */

#endif /* SHADER_H_ */

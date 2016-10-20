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

namespace warp {

	class Shader {
		public:
			Shader(std::string);
			virtual ~Shader();
			GLuint getProgramId();
			// returns the location in the shader, usable by glUniform calls
			GLuint getProjectionMatrixLocation();
			// returns the location in the shader, usable by glUniform calls
			GLuint getModelViewMatrixLocation();
		private:
			// used to replace certain constants (e.g. MAX_LIGHTS) in the shader code before compilation
			void replace(std::string expr, std::string replacement, std::string *vertexCode, std::string *fragmentCode);
			GLuint programId;
			GLuint fragmentId;
			GLuint vertexId;
			GLuint projectionMatrixLocation, modelViewMatrixLocation;

	};

} /* namespace warp */

#endif /* SHADER_H_ */

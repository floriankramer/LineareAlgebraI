/*
 * Shader.cpp
 *
 *  Created on: Dec 11, 2014
 *      Author: dwarf
 */

#include "Shader.h"
#include <iostream>
#include <fstream>
#include "Warp.h"
#include <vector>
#include "LightHandler.h"
#include <stdexcept>
#include <string>

namespace render {


Shader::Shader(std::string src) {
	vertexId = glCreateShader(GL_VERTEX_SHADER);
	fragmentId = glCreateShader(GL_FRAGMENT_SHADER);

	std::string vertexCode;
	std::string fragmentCode;

	std::ifstream in(toOsFileString(src), std::ios::in);
	if(in.is_open()){
		std::string line;
		while(getline(in, line) && line != "\\next"){
			vertexCode += line + "\n";
		}
		while(getline(in, line)){
			fragmentCode += line + "\n";
		}
		in.close();
	}

	std::string falloff = std::to_string(render::light::lightFalloffFactor);
	replace("LIGHT_FALLOFF_FACTOR", falloff, &vertexCode, &fragmentCode);
	replace("NUM_MAX_SHADOWS", std::to_string(light::getNumShadows()), &vertexCode, &fragmentCode);

	GLint result = GL_FALSE;
	int logLength;

	// Compile Vertex Shader
	char const * VertexSourcePointer = vertexCode.c_str();
	glShaderSource(vertexId, 1, &VertexSourcePointer , NULL);
	glCompileShader(vertexId);

	// Check Vertex Shader
	glGetShaderiv(vertexId, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vertexId, GL_INFO_LOG_LENGTH, &logLength);
	if(logLength > 1){
		char * buffer = {};
		glGetShaderInfoLog(vertexId, logLength, NULL, buffer);
		warpLogger.log(std::string(buffer));
	}

	// Compile Fragment Shader
	char const * FragmentSourcePointer = fragmentCode.c_str();
	glShaderSource(fragmentId, 1, &FragmentSourcePointer , NULL);
	glCompileShader(fragmentId);

	// Check Fragment Shader
	glGetShaderiv(fragmentId, GL_COMPILE_STATUS, &result);
	glGetShaderiv(fragmentId, GL_INFO_LOG_LENGTH, &logLength);
	if(logLength > 1){
		GLchar buffer[logLength + 1];
		glGetShaderInfoLog(fragmentId, logLength, NULL, buffer);
		warpLogger.log(std::string(buffer));
	}

	// Link the program
	programId = glCreateProgram();
	glAttachShader(programId, vertexId);
	glAttachShader(programId, fragmentId);
	glLinkProgram(programId);

	// Check the program
	glGetProgramiv(programId, GL_LINK_STATUS, &result);
	glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLength);
	if(logLength > 1){
		GLchar buffer[logLength + 1];
		glGetProgramInfoLog(programId, logLength, NULL, buffer);
		warpLogger.log(std::string(buffer));
	}

	glDeleteShader(vertexId);
	glDeleteShader(fragmentId);

	glUseProgram(programId);
	projectionMatrixLocation = glGetUniformLocation(programId, "projectionMatrix");
	modelViewMatrixLocation = glGetUniformLocation(programId, "modelViewMatrix");
	glUseProgram(0);

	checkForGLError("shader creation of " + src);

}

Shader::~Shader() {
	glDeleteProgram(programId);
	checkForGLError("shader deletion");
}

void Shader::replace(std::string expr, std::string replacement, std::string *vertexCode, std::string *fragmentCode){
	unsigned int pos = 0;
	unsigned int constLength = std::string(expr).length();
	try{
		while((pos = fragmentCode->find(expr, pos + 1)) != static_cast<unsigned int>(std::string::npos)){
			fragmentCode->replace(pos, constLength, replacement);
		}
	}
	catch(std::out_of_range &e){
		warpLogger.log(std::string("EXCEPTION out of range: ") + e.what());
	}
	try{
		while((pos = vertexCode->find("LIGHT_FALLOFF_FACTOR", pos + 1)) != static_cast<unsigned int>(std::string::npos)){
			vertexCode->replace(pos, constLength, replacement);
		}
	}
	catch(std::out_of_range &e){
		warpLogger.log(std::string("EXCEPTION out of range: ") + e.what());
	}
}

GLuint Shader::getProgramId(){
	return programId;
}

GLuint Shader::getProjectionMatrixLocation(){
	return projectionMatrixLocation;
}

GLuint Shader::getModelViewMatrixLocation(){
	return modelViewMatrixLocation;
}

} /* namespace render */

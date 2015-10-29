/*
 * FontRenderer.cpp
 *
 *  Created on: Dec 30, 2014
 *      Author: dwarf
 */

#include "FontRenderer.h"

#include "Warp.h"
#include "os.h"

#include <cmath>
#include <map>
#include <vector>

#include "Mutex.h"
#include <iostream>
#include <cmath>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace render{
namespace font{

Mutex fontMapMutex;


std::map<char, VAOData*> charLocations;
std::map<char, float> charOffsets;

std::vector<std::string> fontsToLoad;

std::string supportedSigns((char) 0 + "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM=?.,;:[]{}<>_1234567890!@#$%^&*()/+-");

FT_Library library;
FT_Face face;

GLuint fontTexture = 0;

void init(){

	int error = FT_Init_FreeType(&library);
	if(error){
		warpLogger.log(std::string("Error while initializing FreeType library. Problems with text rendering will occur"));
	}
	error = FT_New_Face(library, toOsFileString("./gfx/font/arial.ttf").c_str(), 0, &face); //CalligraphyDragonRunes Medium.ttf
	//error = FT_New_Face(library, toOsFileString("./gfx/HussarBold.otf").c_str(), 0, &face); //CalligraphyDragonRunes Medium.ttf
	if(error == FT_Err_Unknown_File_Format){
		warpLogger.log(std::string("Error while loading verdana.ttf, the format is not supported")); //TODO add bitmap loading as alternative
	}
	else if(error == FT_Err_Cannot_Open_Resource){
		warpLogger.log(std::string("Error while loading verdana.ttf, cannot open resource"));
	}
	else if(error){
		warpLogger.log(std::string("Error while loading verdana.ttf: " + std::to_string(error)));
	}
	error = FT_Set_Char_Size(face, 0, 32*64, 0, 0); //defaults to 72 dpi
	unsigned int width = 256;
	unsigned int height = 256;
	unsigned int charHeight = 32;

	unsigned char* buffer = (unsigned char *) malloc(width * height);
	memset(buffer, 0, width * height * sizeof(buffer[0]));

	unsigned int oX = 0, oY = 0;
	for(char c : supportedSigns){ //write the supported signs to the char map
		if(c == 0){
			FT_Load_Char(face, 0xFFFD, FT_LOAD_RENDER);
		}
		else
			FT_Load_Char(face, c, FT_LOAD_RENDER);
		unsigned int glyphWidth = face->glyph->bitmap.width;
		unsigned int length = glyphWidth * face->glyph->bitmap.rows;

		if(width > 0 && height > 0){

			//			int left = face->glyph->bitmap_left;
			int bottom = face->glyph->bitmap_top - face->glyph->bitmap.rows;

			if(bottom < 0)
				bottom -= 5; //TODO find out correct way to align chars (hinting?)


			if(oX + glyphWidth > width){
				oX = 0;
				oY += 1;
			}

			for(unsigned int i = 0; i < length; i++){
				//add 1 to line height to avoid other chars showing with linear interpolation
				buffer[i % (glyphWidth) + i / (glyphWidth) * width + oX + oY * (width * charHeight + width)] = face->glyph->bitmap.buffer[i % glyphWidth + (face->glyph->bitmap.rows - 1 - (i / glyphWidth)) * (glyphWidth)];//face->glyph->bitmap.buffer[i]
				//			buffer[j % (glyphWidth) + j / (glyphWidth) * (n * 32 * 3) + 1] = 0;
				//			buffer[j % (glyphWidth) + j / (glyphWidth) * (n * 32 * 3) + 2] = 0;
			}

			//		createVAO(oX / (width + 0.0), oY / (height + 0.0), (charHeight + 0.0) / height, (glyphWidth + 0.0) / width);

			float minU = oX / (width + 0.0);
			float minV = (oY * charHeight) / (height + 0.0);
			float maxU = minU + (glyphWidth + 0.0) / width;
			float maxV = minV + (charHeight + 0.0) / height;

			charLocations[c] = createVAO(minU, minV, maxU, maxV);
			charOffsets[c] = (bottom + 0.0)/ charHeight;

			oX += glyphWidth + 1; //add one to avoid other chars showing with linear interpolation
			if(oX > width){
				oX = 0;
				oY += 1;
			}
		}
	}
	// write the buffer to a texture;
	glGenTextures(1, &fontTexture);
	glBindTexture(GL_TEXTURE_2D, fontTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, buffer); // GL_TEXTURE_2D, 0, GL_RGBA, n * 32, k * 32, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer
	glBindTexture(GL_TEXTURE_2D, 0);

	free(buffer);

}

void addFontToLoad(std::string s){
	fontMapMutex.lock();
	fontsToLoad.push_back(s);
	fontMapMutex.unlock();
}

void loadNewFonts(){
	fontMapMutex.lock();
	for(std::string s : fontsToLoad){
		loadFont(s);
	}
	fontsToLoad.clear();
	fontMapMutex.unlock();
}

std::string loadFont(std::string path){
	std::string returnValue("unknown error");

	int error;
	error = FT_New_Face(library, toOsFileString(path).c_str(), 0, &face); //CalligraphyDragonRunes Medium.ttf
	//error = FT_New_Face(library, toOsFileString("./gfx/HussarBold.otf").c_str(), 0, &face); //CalligraphyDragonRunes Medium.ttf
	if(error == FT_Err_Unknown_File_Format){
		warpLogger.log(std::string("Error while loading " + path + ", the format is not supported")); //TODO add bitmap loading as alternative
		return std::string("unsupported format");
	}
	else if(error == FT_Err_Cannot_Open_Resource){
		warpLogger.log(std::string("Error while loading " + path + ", cannot open resource"));
		return std::string("can't open resource");
	}
	else if(error){
		warpLogger.log(std::string("Error while loading " + path + ": " + std::to_string(error)));
		return std::string("unknown error");
	}
	error = FT_Set_Char_Size(face, 0, 32*64, 0, 0); //defaults to 72 dpi
	unsigned int width = 256;
	unsigned int height = 256;
	unsigned int charHeight = 32;

	unsigned char* buffer = (unsigned char *) malloc(width * height);
	memset(buffer, 0, width * height * sizeof(buffer[0]));

	unsigned int oX = 0, oY = 0;
	for(char c : supportedSigns){ //write the supported signs to the char map
		if(c == 0){
			FT_Load_Char(face, 0xFFFD, FT_LOAD_RENDER);
		}
		else
			FT_Load_Char(face, c, FT_LOAD_RENDER);
		unsigned int glyphWidth = face->glyph->bitmap.width;
		unsigned int length = glyphWidth * face->glyph->bitmap.rows;

		if(width > 0 && height > 0){

			//			int left = face->glyph->bitmap_left;
			int bottom = face->glyph->bitmap_top - face->glyph->bitmap.rows;

			if(oX + glyphWidth > width){
				oX = 0;
				oY += 1;
			}

			for(unsigned int i = 0; i < length; i++){
				//add 1 to line height to avoid other chars showing with linear interpolation
				buffer[i % (glyphWidth) + i / (glyphWidth) * width + oX + oY * (width * charHeight + width)] = face->glyph->bitmap.buffer[i % glyphWidth + (face->glyph->bitmap.rows - 1 - (i / glyphWidth)) * (glyphWidth)];//face->glyph->bitmap.buffer[i]
				//			buffer[j % (glyphWidth) + j / (glyphWidth) * (n * 32 * 3) + 1] = 0;
				//			buffer[j % (glyphWidth) + j / (glyphWidth) * (n * 32 * 3) + 2] = 0;
			}

			//		createVAO(oX / (width + 0.0), oY / (height + 0.0), (charHeight + 0.0) / height, (glyphWidth + 0.0) / width);

			float minU = oX / (width + 0.0);
			float minV = (oY * charHeight) / (height + 0.0);
			float maxU = minU + (glyphWidth + 0.0) / width;
			float maxV = minV + (charHeight + 0.0) / height;

			charLocations[c] = createVAO(minU, minV, maxU, maxV);
			charOffsets[c] = (bottom + 0.0)/ charHeight;

			oX += glyphWidth + 1; //add one to avoid other chars showing with linear interpolation
			if(oX > width){
				oX = 0;
				oY += 1;
			}
		}
	}
	// write the buffer to a texture;
	if(fontTexture == 0){
		glGenTextures(1, &fontTexture);
		glBindTexture(GL_TEXTURE_2D, fontTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, buffer); // GL_TEXTURE_2D, 0, GL_RGBA, n * 32, k * 32, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else{
		glBindTexture(GL_TEXTURE_2D, fontTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, buffer); // GL_TEXTURE_2D, 0, GL_RGBA, n * 32, k * 32, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	free(buffer);
	returnValue = std::string("success");

	return returnValue;
}

GLuint getFontTexture(){
	return fontTexture;
}

GLuint getVaoId(char c){
	if(charLocations.find(c) != charLocations.end()){
		return charLocations[c]->id;
	}
	else{
		return charLocations[(char)0]->id;
	}
}

VAOData *getVao(char c){
	return charLocations[c];
}

float getOffset(char c){
	return charOffsets[c];
}

}
}


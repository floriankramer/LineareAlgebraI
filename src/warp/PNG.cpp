/*
 * PNG.cpp
 *
 *  Created on: Dec 1, 2014
 *      Author: dwarf
 */

#include "PNG.h"

#include <stdio.h>
#include <string>
#include "os.h"
#include "Warp.h"


PNG::PNG(std::string src) {
	this->src = src;
	width = 0;
	height = 0;
	pixel = NULL;
	depth = 8;
}

PNG::~PNG() {
	free(pixel);
}


bool PNG::load(){
	using namespace std;
	using namespace render;

	unsigned char header[8];

	string s;
	FILE * f = fopen(toOsFileString(src).c_str(), "rb");
	if(!f){
		warpLogger.log(s + src);
		return false;
	}

	fread(header, 1, 8, f);
	if(png_sig_cmp(header, 0, 8)){
		warpLogger.log(string("the file ") + src + " is not a png file");
		return false;
	}

	png_structp pngStructp = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!pngStructp){
		warpLogger.log(string("png_create_read_struct failed for: ") + src);
		return false;
	}

	png_infop pngInfop = png_create_info_struct(pngStructp);

	if(!pngInfop){
		warpLogger.log(string("png_create_info_struct failed for: ") + src);
		return false;
	}

	png_infop endInfo = png_create_info_struct(pngStructp);

	if(setjmp(png_jmpbuf(pngStructp))){
		warpLogger.log(string("initializing init_io failed for png file: ") + src);
		return false;
	}

	png_init_io(pngStructp, f);
	png_set_sig_bytes(pngStructp, 8);

	png_read_info(pngStructp, pngInfop);

	height = png_get_image_height(pngStructp, pngInfop);
	width = png_get_image_width(pngStructp, pngInfop);
	//depth = png_get_bit_depth(pngStructp, pngInfop);
	depth = png_get_rowbytes(pngStructp, pngInfop) / width;
	unsigned int rowbytes = png_get_rowbytes(pngStructp, pngInfop);

	rowbytes += 3 - ((rowbytes - 1) % 4); //somhow handling resolutions other than 2^n. TODO do research on this

	//warpLogger.log(string("image size: ") + string(png_get_image_width(pngStructp, pngInfop)) + ", " + string(png_get_image_height(pngStructp, pngInfop)));

	if(setjmp(png_jmpbuf(pngStructp))){
		warpLogger.log(string("error trying to read png: ") + src);
		return false;
	}

	pixel = (png_byte *) malloc(rowbytes * height * sizeof(png_byte) + 15);

	png_bytep * row_pointers = (png_bytep *) malloc(height * sizeof(png_bytep));

    // set the individual row_pointers to point at the correct offsets of image_data
    for (unsigned int i = 0; i < height; i++)    {
        row_pointers[height - 1 - i] = pixel + i * rowbytes;
    }

//	pixel = (png_bytep *)malloc(sizeof(pixel) * height);
//	for(unsigned int y = 0; y < height; y++){
//		pixel[y] = (png_bytep) malloc(png_get_rowbytes(pngStructp,pngInfop));
//	}

	png_read_image(pngStructp, row_pointers);

	png_destroy_read_struct(&pngStructp, &pngInfop, &endInfo);

	free(row_pointers);

//	png_bytep * temp = (png_bytep *)malloc(png_get_rowbytes(pngStructp,pngInfop));
//	for(unsigned int y = 0; y < height; y++){
//		temp[y] = (png_bytep) malloc(sizeof(pixel) * height);
//	}
//
//	for(unsigned int x = 0; x < width; x++){
//		for(unsigned int y = 0; y < height; y ++){
//			temp[x][y] = pixel[y][x];
//		}
//	}



	//pixel = temp;

	return true;
}

png_byte * PNG::getPixel(){
	return pixel;
}

unsigned int PNG::getHeight(){
	return height;
}

unsigned int PNG::getWidth(){
	return width;
}

unsigned int PNG::getDepth(){
	return depth;
}

unsigned int PNG::getPitch(){
	return depth * width;
}

/*
 * PNG.h
 *
 *  Created on: Dec 1, 2014
 *      Author: dwarf
 */

#ifndef PNG_H_
#define PNG_H_

#include <png.h>
#include <string>


class PNG {
public:
	PNG(std::string);
	virtual ~PNG();

	bool load();
	png_byte * getPixel();
	unsigned int getHeight();
	unsigned int getWidth();
	unsigned int getDepth();
	unsigned int getPitch();
private:
	std::string src;
	unsigned int height, width, depth;
	png_byte * pixel;
};

#endif /* PNG_H_ */

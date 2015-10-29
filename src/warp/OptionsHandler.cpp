/*
 * OptionsHandler.cpp
 *
 *  Created on: Oct 27, 2015
 *      Author: dwarf
 */

#include "OptionsHandler.h"
#include "GraphicsOptions.h"
#include "AudioPlayer.h"
#include "unistd.h"
#include <fstream>
#include "os.h"
#include "Warp.h"
#include "DisplayHandler.h"

void saveOptions(){
	std::ofstream out;
	out.open(toOsFileString("options").c_str(), std::ios::out | std::ios::binary);
	if(out.is_open()){
		char buffer[9];
		buffer[0] = render::options::isFullscreen();

		buffer[1] = render::options::getFBOWidth();
		buffer[2] = render::options::getFBOWidth() >> 8;

		buffer[3] = render::options::getFBOHeight();
		buffer[4] = render::options::getFBOHeight() >> 8;

		uint16_t u16;
		u16 = static_cast<uint16_t>(audio::getGlobalVolume() * 1000);
		buffer[5] = u16 & 0x00FF;
		buffer[6] = (u16 >> 8) & 0x00FF;

		u16 = static_cast<uint16_t>(audio::getMusicVolume() * 1000);
		buffer[7] = u16;
		buffer[8] = u16 >> 8;


		out.write(buffer, 9);
		out.close();
	}
}

void loadOptions(){
	std::ifstream in;
	in.open(toOsFileString("options").c_str(), std::ios::in | std::ios::binary);
	if(in.is_open()){
		char buffer[9];
		std::streamsize s = 0;
		while(s < 9){
			in.read(buffer + s, 9 - s);
			s += in.gcount();
		}
		render::setFullscreen(buffer[0]);
		render::options::setResolution(((buffer[2] << 8) & 0xFF00) + (buffer[1] & 0xFF), ((buffer[4] << 8) & 0xFF00) + (buffer[3] & 0xFF));
		audio::setGlobalVolume((((buffer[5]) & 0x00FF) + ((buffer[6] << 8) & 0xFF00)) / 1000.0);
		audio::setMusicVolume((((buffer[7]) & 0x00FF) + ((buffer[8] << 8) & 0xFF00)) / 1000.0);
		in.close();
	}
}


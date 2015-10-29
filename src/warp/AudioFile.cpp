/*
 * AudioFile.cpp
 *
 *  Created on: Apr 11, 2015
 *      Author: dwarf
 */

#include "AudioFile.h"
#include "Warp.h"

namespace audio{

AudioFile::AudioFile() : filePath(""), format(RAW), mode(BUFFER), channelCount(0), sampleRate(0), bitCount(0), dataBuffer(0), loaded(false), dataSize(0), bufferSize(0), dataRead(0){

}

AudioFile::AudioFile(std::string src) : filePath(src), mode(BUFFER), channelCount(0), sampleRate(0), bitCount(0), dataBuffer(0), loaded(false), dataSize(0), bufferSize(0), dataRead(0){
	std::string t = filePath.substr(filePath.size() - 3, filePath.size());
	if(t == "wav"){
		format = WAV;
	}
	else if(t == "ogg"){
		format = OGG_VORBIS;
	}
	else{
		format = RAW;
	}
}

AudioFile::AudioFile(std::string src, std::string t) : filePath(src), mode(BUFFER), channelCount(0), sampleRate(0), bitCount(0), dataBuffer(0), loaded(false), dataSize(0),  bufferSize(0), dataRead(0){
	if(t == "wav"){
		format = WAV;
	}
	else if(t == "ogg"){
		format = OGG_VORBIS;
	}
	else{
		format = RAW;
	}
}

AudioFile::~AudioFile() {
	if(loaded){
		free(dataBuffer);
	}
}

void AudioFile::setMode(AudioFileMode m){
	mode = m;
}

void AudioFile::open(){
	file.open(filePath.c_str(), std::ios::binary | std::ios::in);
	render::warpLogger.log("opening audio file: " + filePath);

	char buffer[1024];
	for(int i = 0; i < 1024; i++){
		buffer[i] = 0;
	}

	file.read(buffer, 4); //Read type (should be RIFF)
	if(std::string(buffer) != "RIFF"){
		render::warpLogger.log(filePath + " is no sound file! Instead found tag: " + std::string(buffer));
		return;
	}

	uint32_t size = 0; //read file size
	file.read(buffer, 4);
	size |= (buffer[0] << 0) & 0xFF;
	size |= (buffer[1] << 8) & 0xFF00;
	size |= (buffer[2] << 16) & 0xFF0000;
	size |= (buffer[3] << 24) & 0xFF000000;
	render::warpLogger.log("filesize: " + std::to_string(size));

	file.read(buffer, 4); //read WAVE
	if(std::string(buffer) != "WAVE"){
		render::warpLogger.log(filePath + " is no wave file!");
		return;
	}

	file.read(buffer, 4); //read WAVE
	if(std::string(buffer) != "fmt "){
		render::warpLogger.log(filePath + " has no format tag.");
		return;
	}

	uint32_t blockSize = 0;
	file.read(buffer, 4);
	blockSize |= (buffer[0] << 0) & 0xFF;
	blockSize |= (buffer[1] << 8) & 0xFF00;
	blockSize |= (buffer[2] << 16) & 0xFF0000;
	blockSize |= (buffer[3] << 24) & 0xFF000000;
	if(blockSize != 16){
		render::warpLogger.log(filePath + " fmt block size is not 16. unknown format (maybe compressed audio?).");
		return;
	}
	file.read(buffer, blockSize);
	if(buffer[0] != 1){
		render::warpLogger.log(filePath + " unknown wave format.");
		return;
	}
	uint16_t u16 = 0;
	uint32_t u32 = 0;
	u16 |= buffer[2];
	u16 |= (buffer[3] << 8) & 0xFF00;
	channelCount = u16;
	u32 = 0;
	u32 |= (buffer[4]) & 0xFF;
	u32 |= (buffer[5] << 8) & 0xFF00;
	u32 |= (buffer[6] << 16) & 0xFF0000;
	u32 |= (buffer[7] << 24) & 0xFF000000;
	sampleRate = u32;
	u16 = 0;
	u16 |= (buffer[14]) & 0xFF;
	u16 |= (buffer[15] << 8) & 0xFF00;
	bitCount = u16;

	render::warpLogger.log("channel count: " + std::to_string(channelCount));
	render::warpLogger.log("bit count: " + std::to_string(bitCount));
	render::warpLogger.log("samplerate: " + std::to_string(sampleRate));

	for(unsigned int i = 0; i < blockSize; i++){
		buffer[i] = 0;
	}

	file.read(buffer, 4); //read WAVE
	if(std::string(buffer) != "data"){
		render::warpLogger.log(filePath + " has no data tag. Instead: " + std::string(buffer));
		return;
	}
	u32 = 0;
	file.read(buffer, 4);
	u32 |= (buffer[0]) & 0xFF;
	u32 |= (buffer[1] << 8) & 0xFF00;
	u32 |= (buffer[2] << 16) & 0xFF0000;
	u32 |= (buffer[3] << 24) & 0xFF000000;
	dataSize = u32;
	if(mode == BUFFER){
		dataBuffer = static_cast<char *>(malloc(dataSize));
		bufferSize = dataSize;
		file.read(dataBuffer, dataSize);
	}
	else{
		bufferSize = static_cast<uint32_t>(0.07 * sampleRate * bitCount / 8 * channelCount);
		dataBuffer = static_cast<char *>(malloc(bufferSize)); // get enough space for 50 ms of music
	}

	loaded = true;
}

void AudioFile::read(){
	if(file.is_open()){
		file.read(dataBuffer, bufferSize);
		dataRead = file.gcount();
	}
}

bool AudioFile::hasReachedEnd(){
	return file.eof();
}

void AudioFile::close(){
	if(file.is_open()){
		file.close();
	}
}

char *AudioFile::getAudioData(){
	return dataBuffer;
}

uint32_t AudioFile::getChannelCount(){
	return channelCount;
}

uint32_t AudioFile::getSampleRate(){
	return sampleRate;
}

uint32_t AudioFile::getBitCount(){
	return bitCount;
}

uint32_t AudioFile::getDataSize(){
	return dataSize;
}

/**
 * @return the number of actually sensible bytes in the buffer
 */
uint32_t AudioFile::getBufferSize(){
	return mode == STREAM ? dataRead : bufferSize;
}

}

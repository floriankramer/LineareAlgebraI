/*
 * AudioFile.h
 *
 *  Created on: Apr 11, 2015
 *      Author: dwarf
 */

#ifndef AUDIOFILE_H_
#define AUDIOFILE_H_

#include <string>
#include <fstream>

namespace audio{

enum AudioFileMode{
	BUFFER, STREAM
};

enum AudioFileFormat{
	RAW, WAV, OGG_VORBIS
};

class AudioFile {
public:
	AudioFile();
	AudioFile(std::string src);
	AudioFile(std::string src, std::string format);
	virtual ~AudioFile();

	void open();
	void read();
	void close();
	void setMode(AudioFileMode m);
	char *getAudioData();
	uint32_t getChannelCount();
	unsigned int getSampleRate();
	uint32_t getBitCount();
	uint32_t getDataSize();
	uint32_t getBufferSize();
	bool hasReachedEnd();
private:
	std::string filePath;
	AudioFileFormat format;
	AudioFileMode mode;
	std::ifstream file;
	uint32_t channelCount, sampleRate, bitCount;
	char *dataBuffer;
	bool loaded;
	uint32_t dataSize;
	uint32_t bufferSize;
	unsigned int dataRead;

};

}

#endif /* AUDIOFILE_H_ */

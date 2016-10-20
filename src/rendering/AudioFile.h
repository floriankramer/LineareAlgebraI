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
#include <cstdio>
#include <vorbis/vorbisfile.h>

namespace warp{
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

				// In BUFFER mode reads all the data of the audio file.
				// For STREAM mode reads one buffer full of data.
				void read();
				void close();
				void setMode(AudioFileMode m);

				// In BUFFER mode returns the data of the complete file
				// once it has been loaded using read().
				// In STREAM mode returns the last buffer filled with
				// getBufferSize() meaningful bytes read during the last 
				// read() call.
				char *getAudioData();

				uint32_t getChannelCount();
				unsigned int getSampleRate();
				uint32_t getBitCount();

				uint32_t getDataSize();

				// In BUFFER mode returns the size of the whole data
				// stored in the audio file.
				// In STREAM mode returns the amount of data read
				// during the last call to read().
				uint32_t getBufferSize();
				bool hasReachedEnd();
				void printFileInfo();
			private:
				std::string filePath;
				AudioFileFormat format;
				AudioFileMode mode;
				std::ifstream file;
				FILE * cfile;
				uint32_t channelCount, sampleRate, bitCount;
				char *dataBuffer;
				bool loaded;
				uint32_t dataSize;
				uint32_t bufferSize;
				unsigned int dataRead;
				OggVorbis_File *ogg;


		};

	}
}
#endif /* AUDIOFILE_H_ */

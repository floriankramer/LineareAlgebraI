/*
 * AudioPlayer.cpp
 *
 *  Created on: Apr 7, 2015
 *      Author: dwarf
 */

#include "Warp.h"
#include "AudioPlayer.h"
#include "AudioFile.h"
#ifdef AUDIO
#include "AL/al.h"
#include "AL/alc.h"
#endif
#include "unistd.h"

#include <vector>
#include <map>
#include <cmath>


namespace audio{

ALCdevice *device = NULL;
ALCcontext *context = NULL;


std::map<std::string, unsigned int> buffers;
unsigned int sources[NUM_SOURCES];
float sourceVolumes[NUM_SOURCES];
unsigned int musicSources[1];
unsigned int musicBuffers[2];
unsigned int sourceIndex = 0;

bool audioInitialized = false;

float globalVolume = .7;
float musicVolume = .7;
float plainSoundVolume = 0.6;

std::vector<std::string> musicQueue;
bool loopMusicQueue = false;
AudioFile *currentMusicFile = NULL;
unsigned int currentMusicBuffer = 0;
unsigned int currentQueuePosition = 0;

void checkForAlError(std::string pos = "unknown"){
	int error = alGetError();
	if(error != AL_NO_ERROR){
		std::string errormsg = std::to_string(error);
		switch(error){
		case AL_INVALID_ENUM:
			errormsg = "AL_INVALID_ENUM";
			break;
		case AL_INVALID_NAME:
			errormsg = "AL_INVALID_NAME";
			break;
		case AL_INVALID_VALUE:
			errormsg = "AL_INVALID_VALUE";
			break;
		case AL_INVALID_OPERATION:
			errormsg = "AL_INVALID_OPERATION";
			break;
		case AL_OUT_OF_MEMORY:
			errormsg = "AL_OUT_OF_MEMORY";
			break;
		}


		render::warpLogger.log("ERROR: audio at " + pos + ": " + errormsg);
	}
}

void init(){
#ifdef AUDIO
	render::warpLogger.log("initializing audio...");
	device = alcOpenDevice(NULL);
	if(device){
		context = alcCreateContext(device, NULL);
		alcMakeContextCurrent(context);
	}
	else{
		render::warpLogger.log("ERROR: Unable to create OpenAL device");
		return;
	}
	alGetError();
	alGenSources(NUM_SOURCES, sources);
	alGenSources(1, musicSources);
	alGenBuffers(2, musicBuffers);
	int error = alGetError();
	if(error != AL_NO_ERROR){
		render::warpLogger.log("ERROR while generating sources: " + error);
	}
	for(int i : sources){
		alSourcef(i, AL_ROLLOFF_FACTOR, 0.1); //0.25
		alSourcef(i, AL_MAX_DISTANCE, 100);
	}

	for(int i : musicSources){
		alSourcef(i, AL_MIN_GAIN, 0);
		alSourcef(i, AL_MAX_GAIN, 1.0);
		alSourcef(i, AL_GAIN, globalVolume * musicVolume);
		alSourcef(i, AL_PITCH, 1);
		alSourcei(i, AL_SOURCE_RELATIVE, AL_TRUE);
		alSource3f(i, AL_POSITION, 0, 0, 0.0f);
		alSource3f(i, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
		alSourcei(i, AL_LOOPING, AL_FALSE);
	}

	checkForAlError("setup");

	audioInitialized = true;

	for(int i = 0; i < NUM_SOURCES; i++){
		sourceVolumes[i] = 1;
	}

#endif
}

void close(){
#ifdef AUDIO
	if(audioInitialized){
		for(auto a : buffers){
			alDeleteBuffers(1, &a.second);
		}
		alcCloseDevice(device);
	}
#endif
}

int getAudioFormat(AudioFile *f){
	if(f->getChannelCount() == 1){
		if(f->getBitCount() == 8){
			return AL_FORMAT_MONO8;
		}
		else if(f->getBitCount() == 16){
			return AL_FORMAT_MONO16;
		}
		return AL_FORMAT_MONO8;
	}
	else if(f->getChannelCount() == 2){
		if(f->getBitCount() == 8){
			return AL_FORMAT_STEREO8;
		}
		else if(f->getBitCount() == 16){
			return AL_FORMAT_STEREO16;
		}
		return AL_FORMAT_STEREO8;
	}
	return AL_FORMAT_MONO8;
}

void loadSound(std::string path, std::string name){
	if(audioInitialized){
		if(buffers.find(name) == buffers.end()){
			AudioFile f(path);
			f.open(); //load audio data
//			f.printFileInfo();
			unsigned int id;
			alGetError();
			alGenBuffers(1, &id);
			alBufferData(id, getAudioFormat(&f), f.getAudioData(), f.getDataSize(), f.getSampleRate());
			//			alBufferData(id, AL_FORMAT_MONO8, sineTestData, sizeof(sineTestData), 44100);

			checkForAlError("load sound");
			buffers[name]= id;

			f.close();
		}
		else{
			render::warpLogger.log("There already is a buffer named: " + name);
		}
	}
	else{
		//TODO add log verbosity levels
	}
}

int playSound(std::string name, float x, float y, float volume, float pitch, bool looping){
	if(audioInitialized){
		if(buffers.find(name) != buffers.end()){
			render::warpLogger.log("playing sound: " + name);
			unsigned int i = sourceIndex;
			unsigned int s = sources[i];
			alSourcei(s, AL_BUFFER, buffers[name]);
			alSourcef(s, AL_MIN_GAIN, 0);
			alSourcef(s, AL_MAX_GAIN, 1.0);
			alSourcef(s, AL_GAIN, volume * globalVolume);
			alSourcef(s, AL_PITCH, pitch);
			alSourcei(s, AL_SOURCE_RELATIVE, AL_FALSE);
			alSource3f(s, AL_POSITION, x, y, 0.0f);
			alSource3f(s, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
			alSourcei(s, AL_LOOPING, looping ? AL_TRUE : AL_FALSE);
			alSourcePlay(s);

			sourceVolumes[sourceIndex] = volume;

			sourceIndex ++;
			sourceIndex %= NUM_SOURCES;
			return i;
		}
		else{
			render::warpLogger.log("ERROR: no sound named " + name);
		}
	}
	else{

	}
	return -1;
}

int playPlainSound(std::string name, float volume, float pitch, bool looping){
	if(audioInitialized){
		if(buffers.find(name) != buffers.end()){
			render::warpLogger.log("playing sound: " + name);
			unsigned int i = sourceIndex;
			unsigned int s = sources[i];
			alSourcei(s, AL_BUFFER, buffers[name]);
			alSourcef(s, AL_MIN_GAIN, 0);
			alSourcef(s, AL_MAX_GAIN, 1.0);
			alSourcef(s, AL_GAIN, volume * globalVolume * plainSoundVolume);
			alSourcef(s, AL_PITCH, pitch);
			alSourcei(s, AL_SOURCE_RELATIVE, AL_TRUE);
			alSource3f(s, AL_POSITION, 0, 0, 0.0f);
			alSource3f(s, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
			alSourcei(s, AL_LOOPING, looping ? AL_TRUE : AL_FALSE);
			alSourcePlay(s);

			sourceVolumes[sourceIndex] = volume;

			sourceIndex ++;
			sourceIndex %= NUM_SOURCES;
			return i;
		}
		else{
			render::warpLogger.log("ERROR: no sound named " + name);
		}
	}
	else{

	}
	return -1;
}

void updateStreams(){
	if(currentMusicFile == NULL){
		if(musicQueue.size() > 0){
			int state;
			alGetSourcei(musicSources[0], AL_SOURCE_STATE, &state);
			if(state != AL_PLAYING){
				if(currentQueuePosition >= musicQueue.size()){
					if(!loopMusicQueue){
						currentQueuePosition = musicQueue.size() - 1;
					}
					else{
						currentQueuePosition = 0;
					}
				}
				currentMusicFile = new AudioFile(musicQueue[currentQueuePosition]);
				render::warpLogger.log("next song: " + musicQueue[currentQueuePosition]);
				if(!loopMusicQueue){
					musicQueue.erase(musicQueue.begin());
				}
				else{
					currentQueuePosition ++;
				}

				currentMusicFile->setMode(STREAM);
				currentMusicFile->open();
				currentMusicFile->read();
				checkForAlError("before next music");
				alSourceStop(musicSources[0]);
				alSourcei(musicSources[0], AL_BUFFER, 0);
				checkForAlError("play next music 3");
				alBufferData(musicBuffers[0], getAudioFormat(currentMusicFile), currentMusicFile->getAudioData(), currentMusicFile->getBufferSize(), currentMusicFile->getSampleRate());
				checkForAlError("play next music 1");
				currentMusicFile->read();
				alBufferData(musicBuffers[1], getAudioFormat(currentMusicFile), currentMusicFile->getAudioData(), currentMusicFile->getBufferSize(), currentMusicFile->getSampleRate());
				checkForAlError("play next music 2");
				alSourceQueueBuffers(musicSources[0], 2, musicBuffers);
				checkForAlError("play next music 4");
				alSourcePlay(musicSources[0]);
				checkForAlError("play next music");
				currentMusicBuffer = 0;
			}
		}
	}
	else{
		int currentlyUsedBuffer;
		alGetSourcei(musicSources[0], AL_BUFFER, &currentlyUsedBuffer);
		if(musicBuffers[currentMusicBuffer] != currentlyUsedBuffer){
			alSourceUnqueueBuffers(musicSources[0], 1, &musicBuffers[currentMusicBuffer]);
			checkForAlError("buffer stream data1");
			currentMusicFile->read();
			alBufferData(musicBuffers[currentMusicBuffer], getAudioFormat(currentMusicFile), currentMusicFile->getAudioData(), currentMusicFile->getBufferSize(), currentMusicFile->getSampleRate());
			checkForAlError("buffer stream data");
			alSourceQueueBuffers(musicSources[0], 1, &musicBuffers[currentMusicBuffer]);
			int playing;
			alGetSourcei(musicSources[0], AL_SOURCE_STATE, &playing);
			if(playing != AL_PLAYING){
				alSourcePlay(musicSources[0]);
			}
			currentMusicBuffer ++;
			currentMusicBuffer %= 2;
			if(currentMusicFile->hasReachedEnd()){
				currentMusicFile->close();
				delete currentMusicFile;
				currentMusicFile = NULL;
			}
		}
	}
}

void nextSong(){
	if(currentMusicFile != NULL){
		currentMusicFile->close();
		delete currentMusicFile;
		currentMusicFile = NULL;
	}
}

void clearMusicQueue(){
	musicQueue.clear();
}

void queueMusic(std::string path){
	musicQueue.push_back(path);
}

void setMusicQueueLoop(bool b){
	loopMusicQueue = b;
}

void stopSound(int id){
	alSourceStop(sources[id]);
}

void setSoundPosition(int id, float x, float y){
	alSource3f(sources[id], AL_POSITION, x, y, 0);
}

void setListenerPosition(float x, float y){
	alListener3f(AL_POSITION, x, y ,0);
}

void setPitch(int id, float pitch){

}

void setVolume(int id, float volume){

}

void setGlobalVolume(float v){
	globalVolume = v;
	alSourcef(musicSources[0], AL_GAIN, musicVolume * globalVolume);
	//TODO add volume update for other sounds
	int mode;
	for(int i = 0; i < NUM_SOURCES; i++){
		int s = sources[i];
		alGetSourcei(s, AL_SOURCE_RELATIVE, &mode);
		if(mode == 1){
			alSourcef(s, AL_GAIN, sourceVolumes[i] * plainSoundVolume * globalVolume);
		}
		else{
			alSourcef(s, AL_GAIN, sourceVolumes[i] * globalVolume);
		}
	}
}

float getGlobalVolume(){
	return globalVolume;
}

void setMusicVolume(float v){
	musicVolume = v;
	alSourcef(musicSources[0], AL_GAIN, musicVolume * globalVolume);
}

float getMusicVolume(){
	return musicVolume;
}

}


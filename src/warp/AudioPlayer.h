/*
 * AudioPlayer.h
 *
 *  Created on: Apr 7, 2015
 *      Author: dwarf
 */
#ifndef AUDIO_PLAYER_H_QWESAD
#define AUDIO_PLAYER_H_QWESAD

#define AUDIO
#define NUM_SOURCES 32

#include <string>
#include "../os.h"

namespace audio{

void init();
void close();
void loadSound(std::string path, std::string name);

int playSound(std::string name, float x, float y, float volume = 1, float pitch = 1, bool loop = false);
int playPlainSound(std::string name, float volume = 1, float pitch = 1, bool loop = false);
void stopSound(int id);

void queueMusic(std::string path);
void setMusicQueueLoop(bool b);
void updateStreams();
void nextSong();
void clearMusicQueue();

void setSoundPosition(int id, float x, float y);
void setPitch(int id, float pitch);
void setVolume(int id, float volume);

void setListenerPosition(float x, float y);
void setGlobalVolume(float v);
float getGlobalVolume();

void setMusicVolume(float v);
float getMusicVolume();
}

#endif




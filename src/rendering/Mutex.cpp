/*
 * Mutex.cpp
 *
 *  Created on: Dec 2, 2014
 *      Author: dwarf
 */

#include "Mutex.h"
#include <iostream>
#include"os.h"
#ifdef windows
#include <windows.h>
#endif

namespace warp{

Mutex::Mutex() {
#ifdef unix
	pthread_mutex_init(&mutex, NULL);
	pthread_mutex_unlock(&mutex);
#endif
#ifdef windows
	mutex = CreateMutex(NULL, false, NULL);
#endif
}

Mutex::~Mutex() {
#ifdef unix
	pthread_mutex_destroy(&mutex);
#endif
#ifdef windows
	CloseHandle(mutex);
#endif
}


void Mutex::lock(){
#ifdef unix
	pthread_mutex_lock(&mutex);
#endif
#ifdef windows
	WaitForSingleObject(mutex, INFINITE);
#endif
}

void Mutex::unlock(){
#ifdef unix
	pthread_mutex_unlock(&mutex);
#endif
#ifdef windows
	ReleaseMutex(mutex);
#endif
}

}

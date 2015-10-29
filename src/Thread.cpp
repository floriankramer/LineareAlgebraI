/*
 * Thread.cpp
 *
 *  Created on: Nov 30, 2014
 *      Author: dwarf
 */

#include "Thread.h"
#include <iostream>

Thread::Thread(){
	startupFunction = 0;
	thread = 0;
	sleep = false;
#ifdef unix
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);
#endif
#ifdef windows
	event = CreateEvent(NULL, true, false, NULL);
#endif
}

#ifdef unix
Thread::Thread(void*(*f)(void *) ) {
	startupFunction = f;
	thread = 0;
	sleep = false;
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);
}
#endif

#ifdef windows
Thread::Thread(LPTHREAD_START_ROUTINE f){
	startupFunction = f;
	thread = 0;
	sleep = false;
	event = CreateEvent(NULL, true, false, NULL);
}
#endif

Thread::~Thread() {
#ifdef unix
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
#endif
#ifdef windows
	CloseHandle(event);
#endif
}

void Thread::start(){
#ifdef unix
	if(startupFunction != 0){
		pthread_create(&thread, NULL, startupFunction, NULL);
	}
	else{
		thread = pthread_self();
	}
#endif
#ifdef windows
	if(startupFunction != 0){
		if ((thread = CreateThread(NULL, 0, startupFunction, NULL, 0, &id)) == NULL){
			std::cout << "error while creating thread. aborting" << std::endl;
		}
	}
#endif
}

void Thread::wait(){
#ifdef unix
	this->sleep = true;
	pthread_mutex_lock(&mutex);
	while(this->sleep){
		pthread_cond_wait(&cond, &mutex);
	}
	pthread_mutex_unlock(&mutex);
#endif
#ifdef windows
	this->sleep = true;
	while(this->sleep){
		WaitForSingleObjectEx(event, INFINITE, true);
	}
#endif
}

void Thread::wakeUp(){
#ifdef unix
	pthread_mutex_lock(&mutex);
	this->sleep = false;
	pthread_cond_broadcast(&cond);
	pthread_mutex_unlock(&mutex);
#endif
#ifdef windows
	this->sleep = false;
	SetEvent(event);
#endif
}

void Thread::joinCurrentWithThis(){
#ifdef unix
	pthread_join(thread, NULL);
#endif
#ifdef windows
	std::cout << "waiting" << std::endl;
	if (WaitForSingleObjectEx(thread, INFINITE, true) == WAIT_FAILED){
		std::cout << "ERROR at wait for thread: " << GetLastError() << std::endl;
	}
#endif
}

bool Thread::isWaiting(){
	return this->sleep;
}

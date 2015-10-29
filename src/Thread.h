/*
 * Thread.h
 *
 *  Created on: Nov 30, 2014
 *      Author: dwarf
 */

#ifndef THREAD_H_
#define THREAD_H_

#include "os.h"

#ifdef unix
#include <pthread.h>
#endif

#ifdef windows
#include <windows.h>
#endif

class Thread {
public:
#ifdef unix
	Thread(void* (void *));
#endif
#ifdef windows
	Thread(LPTHREAD_START_ROUTINE);
#endif
	Thread();
	virtual ~Thread();
	void start();
	void wait();
	void wakeUp();
	void joinCurrentWithThis();
	bool isWaiting();
private:
	
	bool sleep;
#ifdef unix
	void * (*startupFunction)(void*);
	pthread_t thread;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
#endif
#ifdef windows
	HANDLE thread;
	HANDLE event;
	DWORD id;
	LPTHREAD_START_ROUTINE startupFunction;
#endif
};


#endif /* THREAD_H_ */

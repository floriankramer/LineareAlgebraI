/*
 * Mutex.h
 *
 *  Created on: Dec 2, 2014
 *      Author: dwarf
 */

#ifndef MUTEX_H_
#define MUTEX_H_

#include "os.h"

#ifdef unix
#include <pthread.h>
#endif

#ifdef windows
#include <windows.h>
#endif

class Mutex {
public:
	Mutex();
	virtual ~Mutex();

	void lock();
	void unlock();

private:
#ifdef unix
	pthread_mutex_t mutex;
#endif
#ifdef windows
	HANDLE mutex;
#endif
};

#endif /* MUTEX_H_ */

#include "TimeKeeper.h"

namespace warp{

	TimeKeeper::TimeKeeper() : last(0), current(16), updateFactor(0.13333){
		setTargetFPS(60.0);
	}


	TimeKeeper:: ~TimeKeeper(){

	}



	float TimeKeeper::getUpdateFactor(){
		return updateFactor;
	}

	void TimeKeeper::init(){
#ifdef unix
		clock_gettime(CLOCK_MONOTONIC, &timeStorage);
		pauseTime.tv_sec = 0;
		pauseTime.tv_nsec = 0;
#endif
#ifdef windows
		QueryPerformanceFrequency(&timeBuf);
		perfFreq = timeBuf.QuadPart;
		QueryPerformanceCounter(&timeBuf);
		last = current = timeBuf.QuadPart * 1000000 / perfFreq;

#endif
	}


	void TimeKeeper::beginMeasurement(){
		last = current;
#ifdef unix
		clock_gettime(CLOCK_MONOTONIC, &timeStorage);
		current = 1000000 * timeStorage.tv_sec + timeStorage.tv_nsec / 1000; //time in us
#endif
#ifdef windows
		QueryPerformanceCounter(&timeBuf);	
		current = timeBuf.QuadPart * 1000000 / perfFreq;
#endif

		if(current - last != 0){
			updateFactor = (current - last) / 1000000.0;
		}


	}


	void TimeKeeper::sleep(){
#ifdef unix
		clock_gettime(CLOCK_MONOTONIC, &timeStorage);
		long delta = ((1000000 * timeStorage.tv_sec + timeStorage.tv_nsec / 1000) - current);
		if(delta < 0){
			delta = timeStorage.tv_nsec + (1000000000 - current);
		}
		pauseTime.tv_sec = 0;
		pauseTime.tv_nsec = targetSleepTime - delta * 1000;
		while(nanosleep(&pauseTime, &pauseTime) && errno == EINTR);
#endif
#ifdef windows
		QueryPerformanceCounter(&timeBuf);	
		long t = timeBuf.QuadPart * 1000000 / perfFreq;
		int sleepTime = targetSleepTime - (t - current) / 1000;	
		if(sleepTime < 1){
			sleepTime = 1;
		}
		Sleep(sleepTime);
#endif
	}


	void TimeKeeper::setTargetSleepTime(unsigned long l){
#ifdef unix
		targetSleepTime = l * 1000;
#endif
#ifdef windows
		targetSleepTime = l;
#endif
	}


	void TimeKeeper::setTargetFPS(float fps){
#ifdef unix
		targetSleepTime = (1000000000 / fps);
		
#endif
#ifdef windows
		targetSleepTime = 1000000 / fps;
#endif
	}

}



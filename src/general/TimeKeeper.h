#ifndef WARP_TIME_KEEPER_H_
#define WARP_TIME_KEEPER_H_

#include "os.h"
#ifdef windows
#include <windows.h>
#endif

#ifdef unix
#include <sys/time.h>
#endif

namespace warp{

class TimeKeeper{

	public:
		TimeKeeper();
		virtual ~TimeKeeper();

		void init();
		void beginMeasurement();
		void sleep();
		//set target sleep time in us
		void setTargetSleepTime(unsigned long us);
		void setTargetFPS(float fps);
		float getUpdateFactor();

	private:
		unsigned long targetSleepTime, last, current;
		float updateFactor;
#ifdef unix
		timespec timeStorage;
		timespec pauseTime;
#endif
#ifdef windows
		LARGE_INTEGER timeBuf;
		long long perfFreq;
#endif

};

}

#endif

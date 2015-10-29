/*
 * Logger.h
 *
 *  Created on: Dec 2, 2014
 *      Author: dwarf
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <string>

#include <iostream>
#include "os.h"
#ifdef windows
#include <windows.h>
#endif
#ifdef unix
#include <execinfo.h>
#endif

class Logger {
public:
	Logger(std::string);
	virtual ~Logger();

	template<class T>
	void log(T s){
#ifdef unix
		time_t  timev;
		time(&timev);
		struct tm now;
		char buf[32];
		now = *localtime(&timev);
		strftime(buf, sizeof(buf), "%m-%d-%Y %X", &now);

		std::cout << "[" << name << " " << buf << "] " << s << "\n";
#endif
#ifdef windows
		//std::cout << "[" << name << " " << buf << "] " << s << "\n";*/
		SYSTEMTIME time;
		GetLocalTime(&time);
		char buf[32];

		sprintf(buf, "%d-%d-%d %d:%d:%d", time.wMonth, time.wDay, time.wYear, time.wHour, time.wMinute, time.wSecond);

		std::cout << "[" << getName() << " " << buf << "] " << s << "\n"; //TODO find out what the problem with name is
#endif
	}
	void logCurrentStackTrace();
	void setName(std::string n);
	std::string getName();



private:
	std::string name;
};

#endif /* LOGGER_H_ */

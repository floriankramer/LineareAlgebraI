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
#include <cstdarg>

namespace warp{

  class Logger {
   public:
      Logger(std::string);
      virtual ~Logger();

      template<class T>
        void log(T s) {  
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

      void log(const char *s, ...) {
        va_list v;
        va_start(v, s);

#ifdef unix
        time_t  timev;
        time(&timev);
        struct tm now;
        char buf[32];
        now = *localtime(&timev);
        strftime(buf, sizeof(buf), "%m-%d-%Y %X", &now);

        std::cout << "[" << name << " " << buf << "] ";
        for (int i = 0; s[i] != 0; i++) {
          if (s[i] == '%') {
            i++;
            if (s[i] == 'd') {
              std::cout << va_arg(v, int);
            } else if (s[i] == 'f') {
              std::cout << va_arg(v, double);
            } else if (s[i] == 's') {
              std::cout << va_arg(v, char*);
            };
          } else {
            std::cout << s[i];
          }

        } 
        std::cout << "\n";
#endif
#ifdef windows
        //std::cout << "[" << name << " " << buf << "] " << s << "\n";*/
        SYSTEMTIME time;
        GetLocalTime(&time);
        char buf[32];

        sprintf(buf, "%d-%d-%d %d:%d:%d", time.wMonth, time.wDay, time.wYear, time.wHour, time.wMinute, time.wSecond);

        std::cout << "[" << getName() << " " << buf << "] "; 
        for (int i = 0; s[i] != 0; i++) {
          if (s[i] == '%') {
            i++;
            if (s[i] == 'd') {
              std::cout << va_arg(v, int);
            } else if (s[i] == 'f') {
              std::cout << va_arg(v, double);
            } else if (s[i] == 's') {
              std::cout << va_arg(v, char*);
            };
          } else {
            std::cout << s[i];
          }

        } 
        std::cout << "\n";
#endif
        va_end(v);

      }

      void err(const char *s, ...) {
        va_list v;
        va_start(v, s);

#ifdef unix
        time_t  timev;
        time(&timev);
        struct tm now;
        char buf[32];
        now = *localtime(&timev);
        strftime(buf, sizeof(buf), "%m-%d-%Y %X", &now);

        std::cerr << "[" << name << " " << buf << "] ";
        for (int i = 0; s[i] != 0; i++) {
          if (s[i] == '%') {
            i++;
            if (s[i] == 'd') {
              std::cerr << va_arg(v, int);
            } else if (s[i] == 'f') {
              std::cerr << va_arg(v, double);
            } else if (s[i] == 's') {
              std::cerr << va_arg(v, char*);
            };
          } else {
            std::cerr << s[i];
          }

        } 
        std::cerr << "\n";
#endif
#ifdef windows
        //std::cout << "[" << name << " " << buf << "] " << s << "\n";*/
        SYSTEMTIME time;
        GetLocalTime(&time);
        char buf[32];

        sprintf(buf, "%d-%d-%d %d:%d:%d", time.wMonth, time.wDay, time.wYear, time.wHour, time.wMinute, time.wSecond);

        std::cerr << "[" << getName() << " " << buf << "] "; 
        for (int i = 0; s[i] != 0; i++) {
          if (s[i] == '%') {
            i++;
            if (s[i] == 'd') {
              std::cerr << va_arg(v, int);
            } else if (s[i] == 'f') {
              std::cerr << va_arg(v, double);
            } else if (s[i] == 's') {
              std::cerr << va_arg(v, char*);
            };
          } else {
            std::cerr << s[i];
          }

        } 
        std::cerr << "\n";
#endif
        va_end(v);
      }

      void logCurrentStackTrace();
      void setName(std::string n);
      std::string getName();



   private:
      std::string name;
  };

}

#endif /* LOGGER_H_ */

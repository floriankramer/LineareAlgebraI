/*
 * os.h
 *
 *  Created on: Dec 2, 2014
 *      Author: dwarf
 */

#ifndef OS_H_
#define OS_H_

#ifndef unix
#if defined(__unix__) || defined(__unix)
#define unix
#endif
#endif

#ifndef windows
#ifdef _WIN32
#define windows
#endif
#endif

#include <string>

namespace warp{

std::string toOsFileString(std::string);
std::string getOsPathSeparator();

}

#endif /* OS_H_ */

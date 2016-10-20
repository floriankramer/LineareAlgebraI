/*
* os.cpp
*
*  Created on: Dec 20, 2014
*      Author: dwarf
*/

#include "os.h"
#ifdef windows
#include <algorithm>
#include <windows.h>
#endif
#ifdef unix
#include <unistd.h>
#endif
namespace warp{

std::string getExePath(){
#ifdef windows

	TCHAR buffer[1024];
	DWORD s = GetModuleFileName(NULL, buffer, 1024);
	if(s > 0){
		std::string r = std::string(buffer);
		int i;
		for(i = r.length() - 1; i > 0 && r[i] != '\\'; i--);
		r = r.substr(0, i + 1);
		return r;
	}

	return "./";
#else
#ifdef unix
	char buffer[1024];
	ssize_t s = readlink("/proc/self/exe", buffer, 1023);
	if(s != -1){
		buffer[s] = '\0';
		std::string r = std::string(buffer);
		int i;
		for(i = r.length() - 1; i > 0 && r[i] != '/'; i--);
		r = r.substr(0, i + 1);
		return r;
	}
	else{
		return "./";
	}
#else
	return "./";
#endif
#endif
}

std::string toOsFileString(std::string s){
#ifdef windows
	std::string exePath = getExePath();
	std::replace(exePath.begin(), exePath.end(), '/', '\\');
	if(s.substr(0, getExePath().length()) == exePath){//path is already absolute
		return s;
	}

#else
	if(s.substr(0, getExePath().length()) == getExePath()){ //path is already absolute
		return s;
	}
#endif
	if((s[0] == '.' && s[1] == '/') || s[0] != '/'){
		if(s[0] == '.'){
			s = s.substr(2, s.length());
		}
		s = getExePath() + s;
	}

#ifdef windows
	std::replace(s.begin(), s.end(), '/', '\\');
#endif
	return s;
}

std::string getOsPathSeparator(){
#ifdef windows
	return "\\";
#else
	return "/";
#endif
}

}



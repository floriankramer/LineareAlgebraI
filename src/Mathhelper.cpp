#include "Mathhelper.h"
#include <cmath>

double udist(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2){
	if(x1 > x2 && y1 > y2){
		return hypot(x1 - x2, y1 - y2);
	}
	else if(x1 <= x2 && y1 > y2){
		return hypot(x2 - x1, y1 - y2);
	}
	else if(x1 > x2 && y1 <= y2){
		return hypot(x1 - x2, y2 - y1);
	}
	else{
		return hypot(x2 - x1, y2 - y1);
	}
}

int udiff(unsigned int u1, unsigned int u2){
	if(u1 > u2){
		return u1 - u2;
	}
	else{
		return -static_cast<int>(u2 - u1);
	}
}



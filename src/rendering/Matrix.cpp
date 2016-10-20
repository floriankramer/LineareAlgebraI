/*
 * Matrix.cpp
 *
 *  Created on: Dec 8, 2014
 *      Author: dwarf
 */

#include "Matrix.h"
#include <cmath>
#include <iostream>
#include <cstring>

namespace warp {


Matrix::Matrix() {

	setIdentity();
}

Matrix::~Matrix() {

}

void Matrix::setIdentity(){
	values[0][0] = 1;
	values[1][1] = 1;
	values[2][2] = 1;

	values[0][1] = 0;
	values[0][2] = 0;
	values[1][0] = 0;
	values[1][2] = 0;
	values[2][0] = 0;
	values[2][1] = 0;

//	for(int x = 0; x < 3; x++){
//		for(int y = 0; y < 3; y++){
//			values[x][y] = x == y;
//		}
//	}
}

void Matrix::flip(){
	for(int x = 0; x < 3; x++){
		for(int y = 0; y < 3; y++){
			tempValues[y][x] = values[x][y];
		}
	}
	for(int x = 0; x < 3; x++){
		for(int y = 0; y < 3; y++){
			values[x][y] = tempValues[x][y];
		}
	}
}


void Matrix::quickTranslate(float x, float y){
	values[2][0] += x;
	values[2][1] += y;
}

void Matrix::translate(float x, float y){
	values[2][0] = x * values[0][0] + y * values[1][0] + values[2][0];
	values[2][1] = x * values[0][1] + y * values[1][1] + values[2][1];
	values[2][2] = x * values[0][2] + y * values[1][2] + values[2][2];
}

void Matrix::scale(float x, float y){
	values[0][0] = values[0][0] * x;
	values[0][1] = values[0][1] * x;
	values[0][2] = values[0][2] * x;

	values[1][0] = values[1][0] * y;
	values[1][1] = values[1][1] * y;
	values[1][2] = values[1][2] * y;
}

void Matrix::rotate(float a){
	tempValues[0][0] = (values[0][0] * cos(a) + values[1][0] * sin(a));
	tempValues[0][1] = (values[0][1] * cos(a) + values[1][1] * sin(a));
	tempValues[0][2] = (values[0][2] * cos(a) + values[1][2] * sin(a));

	tempValues[1][0] = (values[1][0] * cos(a) - values[0][0] * sin(a));
	tempValues[1][1] = (values[1][1] * cos(a) - values[0][1] * sin(a));
	tempValues[1][2] = (values[1][2] * cos(a) - values[0][2] * sin(a));

	std::memcpy(values, tempValues, sizeof(GLfloat) * 2 * 3);

/*	values[0][0] = tempValues[0][0];
	values[0][1] = tempValues[0][1];
	values[0][2] = tempValues[0][2];

	values[1][0] = tempValues[1][0];
	values[1][1] = tempValues[1][1];
	values[1][2] = tempValues[1][2];*/
}

void Matrix::print(){
	for(int y = 0; y < 3; y++){
		for(int x = 0; x < 3; x++){
			std::cout << values[x][y] << "\t";
		}
		std::cout << "\n";
	}
	std::cout << std::endl;
}

} /* namespace warp */

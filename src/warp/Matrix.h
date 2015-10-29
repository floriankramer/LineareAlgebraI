/*
 * Matrix.h
 *
 *  Created on: Dec 8, 2014
 *      Author: dwarf
 */

#ifndef MATRIX_H_
#define MATRIX_H_

#include "glewWrapper.h"

namespace render {

/**
 * the matrix is column heavy
 */
class Matrix {
public:
	Matrix();
	virtual ~Matrix();
	void setIdentity();
	/**
	 * this function ignores all previous changes to the matrix, and only works properly if it is used directly after
	 * set identity
	 */
	void quickTranslate(float x, float y);
	void translate(float x, float y);
	void rotate(float);
	void scale(float, float);
	void flip();
	void print();
	GLfloat* getValues(); 
	GLfloat values[3][3];
private:
	GLfloat tempValues[3][3];
};

} /* namespace render */

#endif /* MATRIX_H_ */

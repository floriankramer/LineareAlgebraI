/*
 * MatrixHandler.h
 *
 *  Created on: Dec 13, 2014
 *      Author: dwarf
 */

#ifndef MATRIXHANDLER_H_
#define MATRIXHANDLER_H_

#include "Shader.h"

namespace warp{

	namespace matrix{

		void setupProjectionMatrix(float width, float height);
		void setupProjectionMatrix(float dx, float dy, float sx, float sy);
		void loadIdentity();
		/**
		 * this function ignores all previous changes to the matrix, and only works properly if it is used directly after
		 * set identity
		 */
		void quickTranslate(float x, float y);
		void translate(float, float);
		void rotate(float);
		void scale(float, float);
		void uploadModelViewMatrix();
		void setCameraPosition(float, float);
		void uploadProjectionMatrix();

	}

}

#endif



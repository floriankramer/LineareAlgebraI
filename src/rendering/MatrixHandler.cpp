/*
 * MatrixHandler.cpp
 *
 *  Created on: Dec 13, 2014
 *      Author: dwarf
 */

#include "MatrixHandler.h"

#include "Matrix.h"
#include "glewWrapper.h"
#include "Warp.h"
#include "GraphicsOptions.h"
#include "ShaderHandler.h"

namespace warp{

  namespace matrix{

    Matrix projectionMatrix;
    Matrix modelViewMatrix;

    float cameraX = 0, cameraY = 0;

    void setupProjectionMatrix(float width, float height){
      projectionMatrix.setIdentity();
      projectionMatrix.scale(2 / width, 2 / height);

      uploadProjectionMatrix();
    }

    void setupProjectionMatrix(float dx, float dy, float sx, float sy){
      projectionMatrix.setIdentity();
      projectionMatrix.translate(dx, dy);
      projectionMatrix.scale(2 / sx, 2 / sy);

      uploadProjectionMatrix();
    }

    void uploadProjectionMatrix(){
      glUniformMatrix3fv(shader::getCurrentShader()->getProjectionMatrixLocation(), 1, GL_FALSE, projectionMatrix.values[0]);
    }

    void loadIdentity(){
      modelViewMatrix.setIdentity();
      modelViewMatrix.quickTranslate(-cameraX, -cameraY);
    }

    void quickTranslate(float x, float y){
      modelViewMatrix.quickTranslate(x, y);
    }

    void translate(float x, float y){
      modelViewMatrix.translate(x, y);
    }

    void scale(float x, float y){
      modelViewMatrix.scale(x, y);
    }

    void rotate(float rot){
      if(rot != 0){
        modelViewMatrix.rotate(rot);
      }
    }

    void uploadModelViewMatrix(){
      glUniformMatrix3fv(shader::getCurrentShader()->getModelViewMatrixLocation(), 1, GL_FALSE, modelViewMatrix.values[0]);
    }

    void setCameraPosition(float x, float y){
      cameraX = x;
      cameraY = y;
    }
  }

}

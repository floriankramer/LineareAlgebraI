/*
 * Warp.cpp
 *
 *  Created on: Nov 19, 2014
 *      Author: dwarf
 */

#include "Warp.h"

#define NO_SDL_GLEXT

#include <SDL2/SDL.h>
//#include <SDL2/SDL_opengl.h>
#include <vector>
#include <iostream>
#include <string>
#include <map>

#include <sys/time.h>

#include "RenderableSprite.h"
#include "TextureLoader.h"
#include "GraphicsOptions.h"
#include "DisplayHandler.h"
#include "Shader.h"
#include "glewWrapper.h"
#include "MatrixHandler.h"
#include "ShaderHandler.h"
#include "FBOHandler.h"
#include "LightHandler.h"
#include "FontRenderer.h"
#include "Renderer.h"
#include "os.h"
#include "OptionsHandler.h"
#include "CoordinateConverter.h"
#include "GUIOptions.h"
#include "TimeKeeper.h"

#include <string>
#include <cmath>

#ifdef windows
#include "windows.h"
#endif

namespace warp{

  void doRenderUpdate();
  void loadNewTextures();
  void clearRenderables();
  void cleanUp();
  void createDefaultVaos();
  std::string glErrorToString(GLuint);


  bool shouldRun = true;
  SDL_Event event;
  std::vector<Renderable *> *worldRenderables = new std::vector<Renderable *>();
  std::vector<Renderable *> *hudRenderables = new std::vector<Renderable *>();

  std::vector<VAOData*> vaos;

  float cameraX = 0, cameraY = 0;
  std::vector<PreTexture> preTextures;

  std::map<std::string, Texture> textures;

  Logger warpLogger("Warp");

  GLint defaultVAOId = 0;
  GLuint circleVAOId = 0;

  int fps[7] = {0, 0, 0, 0, 0, 0, 0};
  int fpsIndex = 0;
  bool displayFPS = false;

  float updateFactor = 0.016666667;

  bool multithreaded = false;

  TimeKeeper timeKeeper;



  void init(bool multiThreaded){
    multithreaded = multiThreaded;
    warpLogger.setName("render");
    warpLogger.log("Initializing SDL");
    //  SDL_Init(SDL_INIT_JOYSTICK);
    //  warpLogger.log("was init: " + std::to_string(SDL_WasInit(SDL_INIT_JOYSTICK)));

    try{
      createDisplay();
    }
    catch (const char * msg){
      warpLogger.log("EXCEPTION: " + std::string(msg));

      SDL_Quit();
      return;
    }

    checkForGLError("init");

    options::setPixelFormat(SDL_GetWindowPixelFormat(window));

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glBlendFuncSeparate(GL_SRC_ALPHA, GL_SRC_ALPHA,
    //    GL_ONE_MINUS_SRC_ALPHA, GL_ONE);
    // glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    // glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);

    glClearColor(1, 1, 1, 1);


    //set to 1 for vsync, 0 for max
    SDL_GL_SetSwapInterval(options::getVsync());

    checkForGLError("basic GL setup");

    shader::init();
    checkForGLError("shader initialization");
    fbo::init();
    checkForGLError("vao initialization");
    font::init();
    checkForGLError("font initialization");
    light::init();

    createDefaultVaos();

    warpLogger.log("setup finished");
    warpLogger.log("entering render loop");


    timeKeeper.init();
    options::loadOptions();
  }

  void update(){


    timeKeeper.beginMeasurement();

    fps[fpsIndex] = 1 / timeKeeper.getUpdateFactor();
    fpsIndex = (fpsIndex + 1) % 7;
    updateFactor = timeKeeper.getUpdateFactor(); 

    SDL_PumpEvents(); 

    updateFullscreenState();

    if(displayFPS){
      setRenderTarget(HUD);
      drawString(1, 1, 0.5, "fps: " + std::to_string(static_cast<int>((fps[0] + fps[1] + fps[2] + fps[3] + fps[4] + fps[5] + fps[6]) / 7.0)));
      //drawString(getAspectRatio() - 0.3, 0.05, 0.05, "fps: " + std::to_string(fps[0]));
    }

    if(fbo::recreateFbos){
      fbo::destroy();
      fbo::init();
      fbo::recreateFbos = false;
    }
    if(light::isShouldRebuildShadowMap()){
      light::rebuildShadowMapInternal();
    }

    loadNewTextures();
    font::loadNewFonts();


    doRenderUpdate();

    checkForGLError("render loop");
    if(!options::getVsync() && multithreaded){
      timeKeeper.sleep();
    }
    SDL_GL_SwapWindow(window);

  }

  void cleanup(){
    cleanUp();
    SDL_Quit();
  }


  void doRenderUpdate(){
    if(light::isLightsEnabled()){
      fbo::setFbo(fbo::getColorFboId());
    }
    else{
      fbo::setFbo(fbo::getLightFboId());
    }
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    shader::setCurrentShader(shader::getMainShader());

    matrix::setupProjectionMatrix(options::getVirtualWidth() * 2 * options::getZoom(), options::getVirtualHeight() * 2 * options::getZoom());
    //  matrix::setupProjectionMatrix(options::getAspectRatio() * 20, 20, options::getAspectRatio() * 40, 40);

    float cameraAlignedX = (int)(cameraX / pixelWidth) * pixelWidth;
    float cameraAlignedY = (int)(cameraY / pixelHeight) * pixelHeight;

    shader::uploadCameraLocation(cameraAlignedX, cameraAlignedY);

    matrix::setCameraPosition(cameraAlignedX, cameraAlignedY);

    for (Renderable *r : *worldRenderables){
      r->render();
    }

    if(light::isLightsEnabled()){
      fbo::setFbo(fbo::getLightFboId());

      shader::setCurrentShader(shader::getLightShader());

      glClear(GL_COLOR_BUFFER_BIT);


      glBlendFunc(GL_ONE, GL_ONE);

      glUniform1f(glGetUniformLocation(shader::getLightShaderId(), "screenWidth"), options::getVirtualWidth() / 2);
      glUniform1f(glGetUniformLocation(shader::getLightShaderId(), "screenHeight"), options::getVirtualHeight()/ 2);
      glUniform2f(glGetUniformLocation(shader::getLightShaderId(), "screenCenter"), cameraX, cameraY);

      glBindTexture(GL_TEXTURE_2D, fbo::getFboTextureId(fbo::getColorFboId()));

      int o = 0;
      int i;
      for(i = 0; i <= (light::getLightNumber() - 1) / options::getMaxLights(); i++){
        o += light::uploadLightData(shader::getLightShader(), o);


        glBindVertexArray(defaultVAOId);
        //        glEnableVertexAttribArray(0);
        //        glEnableVertexAttribArray(1);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        //        glDisableVertexAttribArray(0);
        //        glDisableVertexAttribArray(1);
        glBindVertexArray(0);
      }

      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    }
    //render the HUD
    fbo::setFbo(fbo::getLightFboId());  
    if(light::isLightsEnabled()){
      glClear(GL_COLOR_BUFFER_BIT);
    }
    shader::setCurrentShader(shader::getMainShader());
    matrix::setCameraPosition(0, 0);
    matrix::loadIdentity();
    //  matrix::setupProjectionMatrix(options::getAspectRatio() * 2, 2);
    matrix::setupProjectionMatrix(-1, 1, gui::options::getScreenSpaceWidth(), -gui::options::getScreenSpaceHeight());
    glEnable(GL_SCISSOR_TEST);
    for(Renderable *r : *hudRenderables){
      r->render();
    }
    glDisable(GL_SCISSOR_TEST);

    fbo::setFbo(0);
    shader::setCurrentShader(shader::getPostProcessingShader());
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);


    //render the COLOR fbo onto the display
    glBindTexture(GL_TEXTURE_2D, fbo::getFboTextureId(fbo::getLightFboId()));
    glBindVertexArray(defaultVAOId);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    //render the HUD fbo onto the display
    //  glBindTexture(GL_TEXTURE_2D, fbo::getFboTextureId(fbo::HUD));
    //  glBindVertexArray(getDefaultVaoId());
    //  glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);


    light::debug(); 
  }


  void clearRenderables(){
    for (Renderable *r : *worldRenderables){
      delete r;
    }
    for (Renderable *r : *hudRenderables){
      delete r;
    }
    worldRenderables->clear();
    hudRenderables->clear();
  }

  void resetRenderables(){
    for (Renderable *r : *worldRenderables){
      if(!r->getSustain())
        delete r;
    }
    for (Renderable *r : *hudRenderables){
      if(!r->getSustain())
        delete r;
    }
    worldRenderables->clear();
    hudRenderables->clear();
  }

  void cleanUp(){
    shader::destroy();
    fbo::destroy();

    for(auto i : textures){
      Texture t = i.second;
      glDeleteTextures(1, &t.id);
    }

    for(VAOData* v : vaos){
      glDeleteBuffers(1, &v->vboId);
      glDeleteVertexArrays(1, &v->id);
      delete v;
    }

    clearRenderables();

    delete worldRenderables;
    delete hudRenderables;

    destroyDisplay();
    SDL_Quit();
  }

  void addTextureToLoad(PreTexture t){
    lockPreTextures();
    preTextures.push_back(t);
    unlockPreTextures();
  }

  void loadNewTextures(){
    lockPreTextures();
    for(unsigned int i = 0; i < preTextures.size(); i++){

      PreTexture *t = &preTextures.at(i);
      //    warpLogger.log(std::string("loading texture: ") + t->name + " with size: " + std::to_string(t->textureWidth) + " " + std::to_string(t->textureHeight));
      int mode = GL_RGB;
      if(t->data->getDepth() == 4){
        mode = GL_RGBA;
      }
      //    if(t.data->format->BytesPerPixel == 4){
      //      mode = GL_RGBA;
      //    }

      unsigned int id;
      glGenTextures(1, &id);
      glBindTexture(GL_TEXTURE_2D, id);

      //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);
      //glTexImage2D(GL_TEXTURE_2D, 0, mode, t.data->w, t.data->h, 0, mode, GL_UNSIGNED_BYTE, t.data->pixels);
      glTexImage2D(GL_TEXTURE_2D, 0, mode, t->data->getWidth(), t->data->getHeight(), 0, mode, GL_UNSIGNED_BYTE, t->data->getPixel());

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //GL_NEAREST
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //GL_NEAREST

      glBindTexture(GL_TEXTURE_2D, 0);

      //  free(buffer);

      delete t->data;

      //SDL_FreeSurface(t.data);
      //    warpLogger.log("loaded texture");
      Texture texture;
      texture.id = id;
      texture.vao = createVAO((t->srcWidth + 0.0) / t->textureWidth, (t->srcHeight + 0.0) / t->textureHeight);
      textures[t->name] = texture;
    }

    preTextures.clear();
    unlockPreTextures();
  }

  void setCameraPosition(float x, float y){
    cameraX = x;
    cameraY = y;
  }

  void stop(){
    shouldRun = false;
  }

  Texture* getTexture(std::string name){
    if(textures.find(name) != textures.end()){
      return &textures[name];
    }
    return NULL;
  }

  void addRenderable(Renderable *r, RenderTarget t){
    switch(t){
      case WORLD:
        worldRenderables->push_back(r);
        break;
      case HUD:
        hudRenderables->push_back(r);
        break;
      default:
        worldRenderables->push_back(r);
        break;
    }
  }

  void checkForGLError(std::string location){
    int errorValue = glGetError(); //check for GL errors
    if (errorValue != GL_NO_ERROR) {
      warpLogger.log(std::string("GL Error at ") + location + ": " + glErrorToString(errorValue));
    }
  }

  GLint getDefaultVaoId(){
    return defaultVAOId;
  }

  GLuint getCircleVaoId(){
    return circleVAOId;
  }

  void createDefaultVaos(){
    VAOData* t = createVAO(1, 1);
    defaultVAOId = t->id;

    GLuint vboId;

    int points = 32;

    GLfloat *vertexBufferData = new GLfloat[points * 5 + 5];

    vertexBufferData[0] = 0;
    vertexBufferData[1] = 0;
    vertexBufferData[2] = 1;
    vertexBufferData[3] = 0.5;
    vertexBufferData[4] = 0.5;


    for(int i = 1; i < points + 1; i++){
      float a = (i - 1.0) / (points - 1) * 2 * 3.1415926;
      vertexBufferData[i * 5] = cos(a);
      vertexBufferData[i * 5 + 1] = sin(a);
      vertexBufferData[i * 5 + 2] = 1;
      vertexBufferData[i * 5 + 3] = (cos(a) + 1) / 2;
      vertexBufferData[i * 5 + 4] = (sin(a) + 1) / 2;
    }

    glGenVertexArrays(1, &circleVAOId);
    glBindVertexArray(circleVAOId);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &vboId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * (points * 5 + 5), vertexBufferData, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (const GLvoid *)(3 * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    delete [] vertexBufferData;

  }

  VAOData* createVAO(float maxU, float maxV){
    return createVAO(0, 0, maxU, maxV);
  }

  VAOData* createVAO(float minU, float minV, float maxU, float maxV){
    for(VAOData* v : vaos){
      if(v->maxU == maxU && v->maxV == maxV && v->minU == minU && v->minV == minV){
        return v;
      }
    }

    VAOData *vao = new VAOData();

    GLfloat vertexBufferData[] = {
      1, 1, 1, maxU, maxV,
      -1, -1, 1, minU, minV,
      1, -1, 1, maxU, minV,
      1, 1, 1, maxU, maxV,
      -1, 1, 1, minU, maxV,
      -1, -1, 1, minU, minV
    };

    vao->minU = minU;
    vao->minV = minV;
    vao->maxU = maxU;
    vao->maxV = maxV;
    vao->width = maxU - minU;
    vao->height = maxV - minV;

    glGenVertexArrays(1, &vao->id);
    glBindVertexArray(vao->id);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &vao->vboId);
    glBindBuffer(GL_ARRAY_BUFFER, vao->vboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(vertexBufferData)[0], 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(vertexBufferData)[0], (const GLvoid *)(3 * sizeof(vertexBufferData)[0]));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    vaos.push_back(vao);

    checkForGLError("vao creation");

    return vao;
  }


  std::string glErrorToString(GLuint error){
    std::string s;
    if(error == GL_INVALID_ENUM){
      s = "Invalid Enum";
    }
    else if(error == GL_INVALID_VALUE){
      s = "Invalid Value";
    }
    else if(error == GL_INVALID_OPERATION){
      s = "Invalid Operation";
    }
    else if(error == GL_STACK_OVERFLOW){
      s = "Stack Overflow";
    }
    else if(error == GL_STACK_UNDERFLOW){
      s = "Stack Underflow";
    }
    else if(error == GL_OUT_OF_MEMORY){
      s = "Out of Memory";
    }
    else if(error == GL_TABLE_TOO_LARGE){
      s = "Table to Large";
    }
    return s;
  }

  float getCameraX(){
    return cameraX;
  }

  float getCameraY(){
    return cameraY;
  }

  void setShowFPSCounter(bool b){
    displayFPS = b;
  }

  float getUpdateFactor(){
    return updateFactor;
  }
}

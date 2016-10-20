#include "ScreenWorld.h"


#include "warp/GUIOptions.h"
#include "warp/GraphicsOptions.h"
#include "Game.h"
#include "SDL2/SDL.h" 
#include "WorldClient.h"
#include "warp/CoordinateConverter.h"
#include "warp/Warp.h"
#include "warp/Keyboard.h"
#include "warp/Mouse.h"
#include "NodeHive.h"
#include "Server.h"
#include "Client.h"
#include <stdio.h>
#include <cmath>
#include "Options.h"
#include "NodeTunnelHead.h"

namespace ant{

  const float nodeSnapGridSize = 100;
  const float nodeSearchRadius = 50;

  ScreenWorld::ScreenWorld() :
    selectedNode(NULL),
    zoom(1),
    cameraX(0),
    cameraY(0),
    nodesIntersected(0),
    cooldownCanConnectNodes(50),
    newNodeX(0),
    newNodeY(0) {
      setWidth(warp::gui::options::getScreenSpaceWidth());
      setHeight(warp::gui::options::getScreenSpaceHeight());
      setListenToKeys(true);
      setListenToMouse(true);
    }

  ScreenWorld::~ScreenWorld() {

  }

  warp::Color worldLineTempColor(0, 1, 0, 1);
  warp::Color worldLineTempColorBlocked(1, 0, 0, 1);
  warp::Color worldLineTempColorTunnel(0.5, 0.4, 1, 1);



  void ScreenWorld::render(float updateFactor) {
    cooldownCanConnectNodes -= updateFactor * 1000;
    // TODO(florian): move the camera movement to the place it belongs
    if(warp::gui::isKeyDown(SDL_SCANCODE_W)) {
      cameraY += 150 * zoom;	
    }
    if(warp::gui::isKeyDown(SDL_SCANCODE_S)) {
      cameraY -= 150 * zoom;
    }
    if(warp::gui::isKeyDown(SDL_SCANCODE_D)) {
      cameraX += 150 * zoom;
    }
    if(warp::gui::isKeyDown(SDL_SCANCODE_A)) {
      cameraX -= 150 * zoom;
    }
    warp::options::setZoom(zoom);
    warp::setCameraPosition(cameraX, cameraY);
    world::renderWorld();		
    if(selectedNode != NULL) { 
      float *f1 = warp::worldToScreenSpace(selectedNode->getX(), selectedNode->getY());
      float *f3 = warp::worldToScreenSpace(newNodeX, newNodeY); 

      if(nodesIntersected == 0) {
        renderer->drawLine(f1[0], f1[1], f3[0], f3[1], 0.05, worldLineTempColor);
      }
      else if(nodesIntersected == 2) {
        renderer->drawLine(f1[0], f1[1], f3[0], f3[1], 0.05, worldLineTempColorTunnel); 
      }
      else{
        renderer->drawLine(f1[0], f1[1], f3[0], f3[1], 0.05, worldLineTempColorBlocked);
      }
      delete[] f3; 
      delete[] f1;
    } 
    warp::gui::Widget::render(updateFactor);
  }

  void ScreenWorld::findNewNodePosition() {
    float *f2 = warp::gui::getMousePosition();
    float *f3 = warp::screenToWorldSpace(f2[0], f2[1]);
    Node *n = world::getNodeAt(f3[0], f3[1], nodeSearchRadius);
    if (n != nullptr) {
      newNodeX = n->getX();
      newNodeY = n->getY();
    } else {
      newNodeX = std::round(f3[0] / nodeSnapGridSize) * nodeSnapGridSize;
      newNodeY = std::round(f3[1] / nodeSnapGridSize) * nodeSnapGridSize;
    }
  }

  void ScreenWorld::handleKeyEvent(int key, int modifier, bool pressed) {
    if(!pressed) {
      if(key == SDLK_ESCAPE) {
        toMenuScreen();
        server::stop();
        client::stop();
      }	else if(key == SDLK_SPACE) {
        NodeHive *n = world::getMainHive();
        if(n != NULL) {
          cameraX = n->getX();
          cameraY = n->getY();
        }	else{
          cameraX = 0;
          cameraY = 0;
        }
      } else if(key == SDLK_d && (modifier & SDLK_LSHIFT)) {
        options::setDebugRendering(!options::isDebugRendering());
      } else if(key == SDLK_F8) {
        world::setFogOFWarEnabled(!world::isFogOfWarEnabled());
      }	else if(key == SDLK_q) {
        float *m = warp::gui::getMousePosition();
        float *f = warp::screenToWorldSpace(m[0], m[1]);
        Node *n = world::getNodeAt(f[0], f[1]);
        if(n != NULL && n->isInteresting()) {
          n->setCombatPoint(!n->isCombatPoint());
        }
        delete[] f;
      }
    }
  }

  void ScreenWorld::handleMouseWheelEvent(float value) {
    zoom *= pow(1.2, -value);
  }

  void ScreenWorld::handleMouseButtonEvent(int button, float x, float y, bool pressed) {
    float *f = warp::screenToWorldSpace(x, y);
    if(pressed) {
      if(button == 1) {
        Node *n = world::getNodeAt(f[0], f[1], nodeSearchRadius);
        if(n == NULL) {
          //	createNodeAt(f[0], f[1]); 
        }
        else{
          if(n->getDistanceTo(world::getMainHive()) != INT_MAX && n->canBuildPathFrom()) {
            selectedNode = n;	
          }
        }
      }
      else if(button == 2) {
        zoom = 1;
      }
      else if(button == 3) {
        world::deleteNodeAt(f[0], f[1]);
      }
    }
    else{
      if(button == 1) {
        if(selectedNode != NULL) {	
          Node *n = world::getNodeAt(f[0], f[1], 50);
          if(n == NULL) {
            int i = world::countBorderIntersections(f[0], f[1], selectedNode->getX(), selectedNode->getY());
            if(i == 0) { 
              findNewNodePosition(); 
              Node *nn = world::createNodeAt(newNodeX, newNodeY);
              nn->connectTo(selectedNode);
            }
            else if(i == 2) {
              int *r = world::getClosestIntersection(selectedNode->getX(), selectedNode->getY(), f[0], f[1]);
              int *r2 = world::getClosestIntersection(f[0], f[1], selectedNode->getX(), selectedNode->getY());
              NodeTunnelHead *nn = new NodeTunnelHead(r[0], r[1], r2[0], r2[1], f[0], f[1]);
              world::addNode(nn);
              nn->connectTo(selectedNode);
              delete[] r;
              delete[] r2;
            }
          }
          else if(selectedNode != n && !world::intersectsBorder(n->getX(), n->getY(), selectedNode->getX(), selectedNode->getY())) {
            selectedNode->connectTo(n);
            //	n->connectTo(selectedNode);	
          }
          selectedNode = NULL;
        }
      }
    }
    delete[] f;
  }

  void ScreenWorld::handleMouseMotionEvent(float x, float y) {
    if(selectedNode != NULL && cooldownCanConnectNodes < 0) {
      cooldownCanConnectNodes = 50;
      findNewNodePosition();  
      nodesIntersected = world::countBorderIntersections(newNodeX, newNodeY, selectedNode->getX(), selectedNode->getY());
    }
  }
}

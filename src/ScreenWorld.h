
#ifndef SCREEN_WORLD_H
#define SCREEN_WORLD_H

#include <string.h>

#include "warp/Widget.h"
#include "Node.h"

namespace ant{

	class ScreenWorld : public warp::gui::Widget{
		public:
			ScreenWorld();
			virtual ~ScreenWorld();
			void render(float updateFactor);
			virtual void handleKeyEvent(int, int, bool);
			virtual void handleMouseButtonEvent(int button, float x, float y, bool pressed);
			virtual void handleMouseWheelEvent(float value);
			virtual void handleMouseMotionEvent(float x, float y);
		private:

      // recomputes the newNodeX and newNodeY values
      void findNewNodePosition();

			Node *selectedNode;
			float zoom;
			int cameraX, cameraY;
			int nodesIntersected;
			int cooldownCanConnectNodes;

      // stores where a new node would be placed,
      // if the mouse button is released
      float newNodeX, newNodeY;
	};


}
#endif

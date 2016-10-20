
#ifndef SCREEN_MAIN_MENU_H
#define SCREEN_MAIN_MENU_H

#include <string.h>

#include "warp/Widget.h"
#include "warp/WidgetButton.h"

namespace ant{

	class ScreenMainMenu : public warp::gui::Widget, warp::gui::ButtonListener{
		public:
			ScreenMainMenu();
			virtual ~ScreenMainMenu();
			void render(float updateFactor);
			void onButtonPressed(std::string text);
		private:
			warp::gui::WidgetButton buttonQuit;
			warp::gui::WidgetButton buttonSingleplayer;
			warp::gui::WidgetButton buttonMultiplayer;
	};

}
#endif

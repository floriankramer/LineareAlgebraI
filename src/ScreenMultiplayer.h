#ifndef SCREEN_MULTIPLAYER_H_
#define SCREEN_MULTIPLAYER_H_

#include <warp/Widget.h>
#include <warp/WidgetButton.h>
#include <warp/WidgetTextField.h>

namespace ant{

	class ScreenMultiplayer : public warp::gui::Widget, warp::gui::ButtonListener{
		public:
			ScreenMultiplayer();
			virtual ~ScreenMultiplayer();
			void render(float updateFactor);
			void onButtonPressed(std::string text);
		private:
			warp::gui::WidgetButton buttonBack;
			warp::gui::WidgetButton buttonHost;
			warp::gui::WidgetButton buttonJoin;
			warp::gui::WidgetTextField textIp;
			warp::gui::WidgetTextField textName;



	};


}

#endif

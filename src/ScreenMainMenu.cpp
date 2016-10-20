#include "ScreenMainMenu.h"
#include "Game.h"
#include "warp/GUIOptions.h"
#include <stdio.h>
#include "WorldClient.h"


#include "Server.h"
#include "Client.h"

namespace ant{

	ScreenMainMenu::ScreenMainMenu() : buttonQuit("Quit"), buttonSingleplayer("Singleplayer"), buttonMultiplayer("Multiplayer"){
		setWidth(warp::gui::options::getScreenSpaceHeight());
		setHeight(warp::gui::options::getScreenSpaceHeight());

		buttonQuit.setListener(this);
		buttonSingleplayer.setListener(this);
		buttonMultiplayer.setListener(this);

		buttonSingleplayer.setLocation(2, 3);
		buttonMultiplayer.setLocation(2, 5);
		buttonQuit.setLocation(2, 7);


		addWidget(&buttonSingleplayer);
		addWidget(&buttonMultiplayer);
		addWidget(&buttonQuit);
	}

	ScreenMainMenu::~ScreenMainMenu(){

	}

	void ScreenMainMenu::render(float updateFactor){
		renderer->drawRect(0, 0, getWidth(), getHeight(), 0);
		warp::gui::Widget::render(updateFactor);	
	}

	void ScreenMainMenu::onButtonPressed(std::string text){
		if(text == "Quit"){
			quit();
		}
		else if(text == "Singleplayer"){
			server::start();
			client::connectTo("127.0.0.1", "localplayer", 2000);	
	
			toWorldScreen();	
		}
		else if(text == "Multiplayer"){
			toMultiplayerScreen();
		}
	}
}

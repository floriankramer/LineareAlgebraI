#include "ScreenMultiplayer.h"
#include "warp/GUIOptions.h"
#include "Game.h"
#include "Server.h"
#include "Client.h"
#include "WorldClient.h"
#include <cstdio>
#include <string>

namespace ant{

	ScreenMultiplayer::ScreenMultiplayer() : buttonBack("back"), buttonHost("host"), buttonJoin("join"), textIp(6, 1), textName(20, 1){
		setWidth(warp::gui::options::getScreenSpaceHeight());
		setHeight(warp::gui::options::getScreenSpaceHeight());

		buttonHost.setListener(this);
		buttonBack.setListener(this);
		buttonJoin.setListener(this);

		textName.setText("username");
		textIp.setText("127.0.0.1");

		buttonHost.setLocation(2, 2);
		textName.setLocation(2, 5);
		textIp.setLocation(2, 6.5);
		buttonJoin.setLocation(2, 8);
		buttonBack.setLocation(2, 10);


		addWidget(&buttonBack);
		addWidget(&buttonHost);
		addWidget(&buttonJoin);
		addWidget(&textName);
		addWidget(&textIp);
	}

	ScreenMultiplayer::~ScreenMultiplayer(){

	}


	void ScreenMultiplayer::render(float updateFactor){
		warp::gui::Widget::render(updateFactor);	
	}


	void ScreenMultiplayer::onButtonPressed(std::string text){
		if(text == "back"){
			toMenuScreen();
		}
		else if(text == "host"){
			server::start();
			client::connectTo("127.0.0.1", "host", 2000);	
			toWorldScreen();
		}
		else if(text == "join"){
			std::string ip = textIp.getText();
			if(ip.length() > 0){
				if(client::connectTo(ip, textName.getText(), 2000)){
					toWorldScreen();
				}
			}
		}
	}




}



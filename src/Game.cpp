// Copyright 2016 Florian Kramer
//
#include "warp/Warp.h"
#include <cstdio>
#include "warp/Widget.h"

#include "ScreenMainMenu.h"
#include "ScreenWorld.h"
#include "ScreenMultiplayer.h"

#include "warp/Keyboard.h"
#include "warp/LightHandler.h"
#include "warp/GraphicsOptions.h"
#include "warp/TextureLoader.h"
#include "WorldClient.h"
#include "warp/AudioPlayer.h"
#include "warp/TimeKeeper.h"
#include <ctime>
#include "Server.h"
#include "Client.h"
#include "Options.h"
//#include "gperftools/profiler.h"
#include "warp/os.h"

#ifdef windows
#include <windows.h>
#endif

#ifdef unix
#include <sys/time.h>
#endif

namespace ant{

  warp::TimeKeeper timeKeeper;

  float updateFactor;


  bool run = true;
  bool simulate = false;


  warp::gui::Widget *root;
  ScreenMainMenu *mainMenu;
  ScreenWorld *worldScreen;
  ScreenMultiplayer *multiplayerScreen;

  void toWorldScreen();  // TODO temporary debug code, delete

  void startGame() {
    std::srand(std::time(NULL));

    options::loadOptions();	

    warp::options::setVirtualHeight(1000);	
    warp::init(false);
    warp::setShowFPSCounter(true);
    //warp::audio::init();
    warp::gui::initKeyboard();
    warp::light::setLightEnabled(false);

    warp::audio::init();
    //	warp::audio::queueMusic("./music/YogaMusic.ogg");
    //	warp::audio::setMusicQueueLoop(true);

    root = new warp::gui::Widget();
    root->setWidth(200);
    root->setHeight(20);

    warp::loadTexture("steampunk_heart1", "gfx/base/steampunk_heart1_f.png");

    mainMenu = new ScreenMainMenu();
    worldScreen = new ScreenWorld();
    multiplayerScreen = new ScreenMultiplayer();

    root->addWidget(mainMenu);
    //root->addWidget(worldScreen);

    //generateWorld(4);


    timeKeeper.setTargetFPS(60);
    timeKeeper.init();

//    server::start();
//    client::connectTo("127.0.0.1", "localplayer", 2000);	
//    toWorldScreen(); 

    // main loop
    while(run){
      timeKeeper.beginMeasurement();
      updateFactor = timeKeeper.getUpdateFactor();

      if(client::isConnected()){
        client::processPackets();
      }

      root->handleInput();
      root->render(updateFactor);
      if(simulate){
        world::updateWorld(updateFactor);
      }
      warp::audio::updateStreams();
      warp::update();
      warp::resetRenderables();
      timeKeeper.sleep();
    }


    delete root;
    delete mainMenu;
    delete worldScreen;
    warp::cleanup();
    warp::audio::close();
    world::cleanupWorld();

  }


  void quit(){
    run = false;
  }


  void toWorldScreen(){
    root->removeWidget(mainMenu);
    root->removeWidget(multiplayerScreen);
    root->addWidget(worldScreen);
    worldScreen->handleKeyEvent(SDLK_SPACE, 0, false);
    warp::gui::setFocus(worldScreen);
    simulate = true;
  }

  void toMenuScreen(){
    root->removeWidget(worldScreen);
    root->removeWidget(multiplayerScreen);
    root->addWidget(mainMenu);
    simulate = false;
  }

  void toMultiplayerScreen(){
    root->removeWidget(worldScreen);
    root->removeWidget(mainMenu);
    root->addWidget(multiplayerScreen);
    simulate = false;
  }

}

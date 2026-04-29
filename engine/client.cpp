#include "engine.hpp"
#include "scene.hpp"
#include "network/NetClient.hpp"
#include "SDLGraphicsManager.hpp"
#include <cereal/types/polymorphic.hpp>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "assets.hpp"
#include "IOSystem.hpp"

unsigned int _last_receive;

bool _server = false;
bool _draw = true;

void client_init(IOSystem io_system_context){
  SDL_Init(SDL_INIT_EVERYTHING);
  TTF_Init();

  auto graphics_interface = io_system_context.getGraphicsManager();
  graphics_interface.createWindow(600, 800);
  graphics_interface.setDraw(true);

  auto sound_manager = io_system_context.getSoundManager();

  loadAssets(sound_manager, graphics_interface);
  init_engine(false);

  graphics_interface.renderSplashScreen();

  std::this_thread::sleep_for(std::chrono::seconds(1));
}


void
client_loop(IOSystem& io_system_context)
{

  timespec t1, t2;
  t2.tv_nsec = 0;
  t2.tv_sec = 0;

  while (!_halt) {

    t1 = t2;
    // Sleep until next tick
    do{
      double time_to_sleep = (1e9/TICK_RATE - (t2.tv_nsec - t1.tv_nsec + 1e9 * (t2.tv_sec - t1.tv_sec)))*1e-6;
      if (clock_gettime(CLOCK_REALTIME, &t2) == -1)
        log_message(ERR, "Failed to get time");
      SDL_Delay(time_to_sleep*.8);
    }
    while(t2.tv_nsec - t1.tv_nsec +
          1e9 * (t2.tv_sec - t1.tv_sec) < 1e9 / TICK_RATE);

    // Perform client tick
    client_entry(io_system_context);
  }
}

void client_entry(IOSystem& io_system_context) {
  /* Lock _scene_mutex to protect _pScene from other threads */
  LOCK_GUARD(_scene_mutex);

  _net_client->pollServer();
  if (_pScene) {
    _pScene->update();
    handle_input(io_system_context);
  }
  if (_draw){
    auto gfx = io_system_context.getGraphicsManager();
    gfx.resetFrameBuffers();
    _pScene->draw();
    gfx.drawScreen();
  }
  _tick++;

  _pNewScene = _pScene->getNextScene();
  if (_pNewScene != nullptr) {
    _pScene = _pNewScene;
    _pNewScene = nullptr;
  }
}

void
engine_start_game()
{
  return;
}

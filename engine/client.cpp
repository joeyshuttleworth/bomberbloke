#include "engine.hpp"
#include "network/NetClient.hpp"
#include <cereal/types/polymorphic.hpp>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

unsigned int _last_receive;
bool _draw = true;
bool _server = false;

void
client_loop()
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
    client_entry();
  }
}

void client_entry() {
  /* Lock _scene_mutex to protect _pScene from other threads */
  LOCK_GUARD(_scene_mutex);

  _net_client->pollServer();
  if (_pScene) {
    _pScene->update();
    handle_input();
  }
  if (_draw)
    draw_screen();
  _tick++;

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

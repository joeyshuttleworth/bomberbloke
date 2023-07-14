#include "engine.hpp"
#include "network/NetServer.hpp"
#include <memory>

bool _server = true;
bool _draw = false;
bool _debug_player = false;
unsigned int _ping_time = 0;

void server_loop(short port, std::string masterServerAddress, bool debug){
  if(debug){
    _debug_player = true;
    log_message(INFO, "DEBUG Mode: On");
  }

  timespec t1, t2;
  t2.tv_nsec = 0;
  t2.tv_sec = 0;

  _net_server->setMasterServerAddress(masterServerAddress);
  _net_server->init(port);

  while (!_halt) {
    t1 = t2;
    do {
      // Time to sleep until next tick (in nanoseconds)
      double time_to_sleep = (1e9/TICK_RATE - (t2.tv_nsec - t1.tv_nsec + 1e9 * (t2.tv_sec - t1.tv_sec)))*1e-6;

      // If we're not playing, sleep for longer
      if(_pScene){
        if(_pScene->getState() == STOPPED)
          time_to_sleep = 1000;
      }

      // SDL_Delay might take too long so only sleep for half the time
      SDL_Delay(time_to_sleep*.75);
      if (clock_gettime(CLOCK_REALTIME, &t2) == -1)
        log_message(ERR, "Failed to get time");
    } while (t2.tv_nsec - t1.tv_nsec +
               1e9 * (t2.tv_sec - t1.tv_sec) < 1e9 / TICK_RATE);
    _net_server->update();
    if (_tick % (5 * TICK_RATE) == 0) {
      _ping_time = _tick;
    }
    /* Lock _scene_mutex to protect _pScene from other threads */
    {
      const std::lock_guard<std::mutex> lock(_scene_mutex);
      if (!_pScene)
        _pScene = std::make_shared<scene>(10, 10);
      _pScene->update();
      draw_screen();
      _tick++;
      if (_tick % 1000 == 0)
        _net_server->syncPlayers();
      handle_input();
    }
    if(_debug_player && _player_list.size() == 0) { // If empty add a dummy player in debug mode
      server_add_debug_player();
    }
    if (_pScene->getNewGame() && _player_list.size() > 1) {
      new_game("");
    }
  }
  return;
}

#include "engine.hpp"
#include "server.h"
#include "network/NetServer.hpp"
#include <memory>

bool _server = true;
bool _draw   = false;
unsigned int _ping_time = 0;

void server_loop(){
  timespec t1, t2;
  t2.tv_nsec = 0;
  t2.tv_sec  = 0;

  if(!_net_server.init_enet())
    return;

  while(!_halt){
    t1 = t2;
    do{
      SDL_Delay(0);
      if(clock_gettime(CLOCK_REALTIME, &t2)==-1)
        log_message(ERROR, "Failed to get time");
    }while(t2.tv_nsec - t1.tv_nsec + (float)(BILLION * (t2.tv_sec - t1.tv_sec))  < (float)BILLION/(float)TICK_RATE);
    _net_server.update();
    if(_tick % (5 * TICK_RATE) == 0){
      _ping_time = _tick;
    }
    /* Lock _scene_mutex to protect _pScene from other threads */
    {const std::lock_guard<std::mutex> lock(_scene_mutex);
    if(!_pScene)
      _pScene = std::make_shared<scene>(10,10);
    _pScene->update();
    draw_screen();
    _tick++;
    if(_tick%1000==0)
      _net_server.syncPlayers();
    handle_input();
    }
    if(_pScene->getNewGame() && _player_list.size() > 1)
      new_game("");
  }
  return;
}


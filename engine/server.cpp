#include "engine.hpp"
#include "server.h"
#include "network/NetServer.hpp"

bool _server = true;
bool _draw   = false;
unsigned int _ping_time = 0;

void server_loop(){
  timespec t1, t2;
  t2.tv_nsec = 0;
  t2.tv_sec  = 0;

  // Initialise network
  NetServer net_server;
  if(!net_server.init_enet)
    return;

  while(!_halt){
      log_message(0, "Sttart");
      net_server.poll();
    t1 = t2;
    do{
      SDL_Delay(0);
      if(clock_gettime(CLOCK_REALTIME, &t2)==-1)
        log_message(ERROR, "Failed to get time");
    }while(t2.tv_nsec - t1.tv_nsec + (float)(BILLION * (t2.tv_sec - t1.tv_sec))  < (float)BILLION/(float)TICK_RATE);
    
    net_server.poll();

    if(_tick % (5 * TICK_RATE) == 0){
      _ping_time = _tick;
    }
    if(_state == PLAYING){
      for(auto i=_level.mActors.begin(); i!=_level.mActors.end(); i++){
        auto prev=i;
        i++;
        (*prev)->update();
      }
      _level.mActors.remove_if([](std::shared_ptr<actor>a){return a->mRemove;});
      handle_movement();
    }
    draw_screen();
      _tick++;
  }

}


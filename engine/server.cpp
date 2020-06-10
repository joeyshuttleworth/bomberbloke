#include "engine.h"
#include "server.h"
#include "network/NetServer.h"
bool _server = true;
bool _draw   = false;
unsigned int _ping_time = 0;
const std::vector<command_binding> _default_bindings;


void server_loop(){
  unsigned int last_tick, current_tick=0;
  int delay;
  timespec t1, t2;
  t2.tv_nsec = 0;
  t2.tv_sec  = 0;

  // Initialise network
  NetServer server;


  while(!_halt){
    t1 = t2;
    do{
      SDL_Delay(0);
      if(clock_gettime(CLOCK_REALTIME, &t2)==-1)
        log_message(ERROR, "Failed to get time");
    }while(t2.tv_nsec - t1.tv_nsec + (float)(BILLION * (t2.tv_sec - t1.tv_sec))  < (float)BILLION/(float)TICK_RATE);
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
  // Explicity call deconstructor (probably not necessary)
  //server.~Server();
}

void send_player_list(){
 //  char *player_list;
  
//   for(auto i = _client_list.begin();  i!= _client_list.end(); i++){

//   }
//     return;
// } 
//   void engine_start_game(){
//     log_message(INFO, (const char*)"Server starting game\n");
//     return;
  }

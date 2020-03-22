#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "engine.h"

unsigned int _last_receive;
bool _draw = true;
bool _server = false;

void client_loop(){
  unsigned int current=0, last;
  int delay;
  _state = PLAYING;

  while(!_halt){
     last = current;
     delay=(1000/TICK_RATE) - current + last;
     if(delay>0){
       SDL_Delay(delay);
     }
     handle_input(&_level);
     handle_movement();
     draw_screen();
     _tick++;
  }
}


void engine_new_game(std::string tokens){
  log_message(INFO, (char*)"New game ready to start...\n");
  _state = PAUSED;
  new_game(tokens);
  _level = level();
  return;
}

void engine_start_game(){
  return;
}

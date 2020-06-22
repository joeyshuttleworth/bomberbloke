#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "network/NetClient.hpp"
#include "engine.hpp"

unsigned int _last_receive;
bool _draw = true;
bool _server = false;
std::string _serverIP = "127.0.0.1";
enet_uint16 _port = 8888;

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
     handle_input();
     handle_movement();
     draw_screen();
     gameUpdate();
     _tick++;
  }
}

void setAddress(std::string serverAddress = "127.0.0.1", enet_uint16 port=8888){
  _serverIP = serverAddress;
  _port = port;
  _server = true;
}

void engine_new_game(std::string tokens){
  log_message(INFO, (char*)"New game ready to start...\n");
  _state = PAUSED;
  new_game(tokens);
  // _pLevel = level();
  return;
}

void engine_start_game(){
  return;
}

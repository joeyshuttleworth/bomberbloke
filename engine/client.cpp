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
     /* Lock _scene_mutex to protect _pScene from other threads */
     const std::lock_guard<std::mutex> lock(_scene_mutex);
     _net_client.pollServer();
     if(_pScene){
       _pScene->update();
       handle_input();
     }
     if(_draw)
       draw_screen();
     _tick++;

     if (_pNewScene != nullptr) {
       _pScene = _pNewScene;
       _pNewScene = nullptr;
     }
  }
}

void setAddress(std::string serverAddress = "127.0.0.1", enet_uint16 port=8888){
  _serverIP = serverAddress;
  _port = port;
  _server = true;
}

void engine_start_game(){
  return;
}

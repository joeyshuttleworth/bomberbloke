#include "cereal/archives/json.hpp"
#include "bomberbloke.h"
#include "ServerInfo.hpp"
#include "ServerInfoEvent.hpp"
#include <SDL2/SDL.h>

int    _default_bomb_time = DEFAULT_BOMB_TIMER;
double _bloke_size[2]     = {DEFAULT_BLOKE_SIZE, DEFAULT_BLOKE_SIZE};
std::vector<int> _spawn_points = {5,5};
int colours[50][3];

int main (int argc, char **argv){
  log_message(INFO, "Bomberbloke client starting...");
  init_engine();
  //_local_player_list.push_back(local_p("nickname"));
  ServerInfoEvent e(_server_info, _player_list);
  cereal::JSONOutputArchive oArchive(std::cout);
  //Pretend we're loading - makes output look nice. 
  sleep(1);
  oArchive(e);
  std::cout << std::endl;
  server_loop();
  // _level.spawn_points = _spawn_points;
  SDL_Quit();
  return 0;  
}

void new_game(std::string arguments){
  int c=0;
  _level.mSpawnPoints = _spawn_points;
  /*for(auto i = _client_list.begin(); i != _client_list.end(); i++){
     bloke b = bloke();
    if(c < (double)(_level.spawn_points.size())/2){
      b.position[0] = _level.spawn_points[c*2];
      b.position[1] = _level.spawn_points[c*2+1];
      _level.mActors.push_back(b);
      i->character = &_level.mActors.back();
      SDL_SetRenderTarget(_renderer, i->character->sprite);
      SDL_SetRenderDrawColor(_renderer, 0x00, 0x00, 0xFF, 0xFF);
      SDL_RenderFillRect(_renderer, NULL);
      SDL_SetRenderTarget(_renderer, NULL);
    }
    else{
      log_message(ERROR, (char*)"Not enough spawn points for all players\n");
      break;
    } 
  }*/
  return;
}

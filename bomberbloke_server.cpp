#include "bomberbloke.h"
#include <SDL2/SDL.h>

int    _default_bomb_time = DEFAULT_BOMB_TIMER;
double _bloke_size[2]     = {DEFAULT_BLOKE_SIZE, DEFAULT_BLOKE_SIZE};
std::vector<int> _spawn_points = {0,0,0,9,9,9,9,0};
int colours[50][3];

int main (int argc, char **argv){
  log_message(INFO, "Bomberbloke client starting...");
  init_engine();
  _local_player_list.push_back(local_p("nickname"));
  server_loop();
  _level.spawn_points = _spawn_points;
  SDL_Quit();
  return 0;  
}

void new_game(std::string arguments){
  int c=0;
  _level = level(10,10);
  _level.spawn_points = _spawn_points;
  for(auto i = _client_list.begin(); i != _client_list.end(); i++){
    bloke b = bloke();
    if(c < (double)(_level.spawn_points.size())/2){
      b.position[0] = _level.spawn_points[c*2];
      b.position[1] = _level.spawn_points[c*2+1];
      _level.actor_list.push_back(b);
      i->character = &_level.actor_list.back();
      SDL_FillRect(i->character->sprite, NULL, SDL_MapRGB(i->character->sprite->format, 0,0,0));
    }
    else{
      log_message(ERROR, (char*)"Not enough spawn points for all players\n");
      break;
    }
  }
  draw_screen();
  return;
}

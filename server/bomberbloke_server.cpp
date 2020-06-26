#include <cereal/archives/json.hpp>
#include "bomberbloke.h"
#include "ServerInfo.hpp"
#include "ServerInfoEvent.hpp"
#include <SDL2/SDL.h>
#include <random>
#include <array>
#include <memory>
#include "woodenCrate.hpp"

/* Register our actors with cereal */
CEREAL_REGISTER_DYNAMIC_INIT(actors)

int    _default_bomb_time = DEFAULT_BOMB_TIMER;
double _bloke_size[2]     = {DEFAULT_BLOKE_SIZE, DEFAULT_BLOKE_SIZE};
std::vector<int> _spawn_points = {5,5};
int colours[50][3];

int main (){

  log_message(INFO, "Bomberbloke server starting...");

  init_engine();
  server_loop();
  SDL_Quit();
  SDL_Delay(1000);
  return 0;
}

void new_game(std::string){

  std::random_device rd;
  std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
  std::uniform_int_distribution<> distrib(0, 9);

  int blocks[10][10];

  memset(blocks, 0, sizeof(int)*10*10);

  for(unsigned int i = 0; i < 5; i++){
    bool set = false;
    for(int j = 0; j<10000; j++){
      int xpos = distrib(gen);
      int ypos = distrib(gen);
      if(blocks[xpos][ypos] != SPAWN_POINT){
        blocks[xpos][ypos] = SPAWN_POINT;
        /* make space around the spawn point */
          if(xpos + 1 < 10){
            blocks[xpos+1][ypos] = RESERVED;
          }
          if(xpos - 1 >= 0){
            blocks[xpos-1][ypos] = RESERVED;
          }
          if(ypos + 1 < 10){
            blocks[xpos][ypos+1] = RESERVED;
          }
          if(ypos - 1 >= 0){
            blocks[xpos][ypos-1] = RESERVED;
          }
          set = true;
          break;
      }
    }
    if(!set){
      log_message(ERROR, "failed to generate spawn points");
    }
  }

  /* Fill in the other blocks - could be wooden crates or other types*/

  for(int i = 0; i < 10; i++){
    for(int j = 0; j < 10; j++){
      if(blocks[i][j] == EMPTY){
        blocks[i][j] = ACTOR_WOODEN_CRATE;
        _pScene->mActors.push_back(std::shared_ptr<woodenCrate>(new woodenCrate(i, j)));
      }
    }
  }

  return;
}

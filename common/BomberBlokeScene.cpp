#include "BomberBlokeScene.hpp"
#include <random>
#include <memory>
#include <algorithm>
#include "engine.hpp"
#include "scene.hpp"

void BomberBlokeScene::draw(Camera *cam){
  if(!cam){
    return;
    log_message(ERROR, "null camera");
  }

  drawActors(cam);
  drawParticles(cam);
  drawHud(cam);
  int zoom = cam->GetZoom();
  /* Draw a grid */
  for(int i = 0; i<=10; i++){
    SDL_RenderDrawLine(_renderer, i * zoom, 0, i * zoom, mDimmension[1]*zoom);
    SDL_RenderDrawLine(_renderer, 0, i*zoom, mDimmension[0]*zoom, i*zoom);
  }
}

void BomberBlokeScene::LogicUpdate(){
  // count blokes

  if(mState == PAUSED || mState == STOPPED)
    return;

  int number_of_blokes = std::count_if(mActors.begin(), mActors.end(), [](std::shared_ptr<actor> i) -> bool {return i->getType() == ACTOR_BLOKE;});
  bool new_game = false;
  switch(number_of_blokes){
  case 0:{
      log_message(INFO, "All blokes are dead.");
      new_game = true;
      mState = STOPPED;
      break;
  }
  case 1:{
      log_message(INFO, "Someone has won");
      new_game = true;
      mState = STOPPED;
      break;
  }
  default:
    break;
  }
  // if(new_game)
  //   _pScene = std::make_shared<BomberBlokeScene>(10, 10);
}

BomberBlokeScene::BomberBlokeScene(int size_x, int size_y) : scene(size_x, size_y){
  mState = PLAYING;
  if(_server){
    std::random_device rd;
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> distrib(0, 9);
    std::vector<std::array<int, 2>> spawn_points;
    spawn_points.reserve(5);

    int blocks[size_x][size_y];
    memset(blocks, 0, sizeof(int)*size_x*size_y);

    for(unsigned int i = 0; i < 5; i++){
      bool set = false;
      for(int j = 0; j<10000; j++){
        int xpos = distrib(gen);
        int ypos = distrib(gen);
        if(blocks[xpos][ypos] != SPAWN_POINT){
          blocks[xpos][ypos] = SPAWN_POINT;
          spawn_points.push_back({xpos, ypos});
          /* make space around the spawn point */
          if(xpos + 1 < size_x){
            blocks[xpos+1][ypos] = RESERVED;
          }
          if(xpos - 1 >= 0){
            blocks[xpos-1][ypos] = RESERVED;
          }
          if(ypos + 1 < size_y){
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
          addActor(std::shared_ptr<woodenCrate>(new woodenCrate(i, j)));
        }
      }
    }

    /* populate the map with players */
    auto iter = _player_list.begin();
    for(unsigned int i = 0; i < 5; i++){
      if(iter == _player_list.end())
        break;
      addActor(std::shared_ptr<bloke>(new bloke(spawn_points[i][0], spawn_points[i][1], true)));
      (*iter)->setCharacter(mActors.back());
      iter++;
    }
  }
  log_message(INFO, "no. actors " + std::to_string(mActors.size()));
  return;
}

#include "BomberBlokeScene.hpp"
#include <random>
#include <memory>
#include <algorithm>
#include "engine.hpp"
#include "scene.hpp"
#include "bomberbloke.h"

static void hudTestFn1() {
  std::cout << "clicked centre button" << std::endl;
}

static void hudTestFn2() {
  std::cout << "clicked left button" << std::endl;
  handle_system_command(split_to_tokens("nickname dave1"));
  handle_system_command(split_to_tokens("open 127.0.0.1"));
}

static void hudTestFn3() {
  std::cout << "clicked right button" << std::endl;
  handle_system_command(split_to_tokens("nickname dave2"));
  handle_system_command(split_to_tokens("open 127.0.0.1"));
}


void BomberBlokeScene::draw(){
  if(!mpCamera){
    return;
    log_message(ERROR, "null camera");
  }

  SDL_SetRenderTarget(_renderer, mpCamera->getFrameBuffer());
  // SDL_SetRenderTarget(_renderer, nullptr);
  SDL_SetRenderDrawColor(_renderer, 0x00, 0x10, 0xff, 0xff);
  // SDL_RenderFillRect(_renderer, nullptr);
  SDL_RenderClear(_renderer);

  int zoom = mpCamera->GetZoom();

  SDL_SetRenderDrawColor(_renderer, 0x10, 0x10, 0x10, 0xa0);

  /* Draw a grid */
  for(int i = 0; i<=10; i++){
    SDL_RenderDrawLine(_renderer, i * zoom, 0, i * zoom, mDimmension[1]*zoom);
    SDL_RenderDrawLine(_renderer, 0, i*zoom, mDimmension[0]*zoom, i*zoom);
  }

  drawActors();
  drawParticles();
  drawHud();


  mpCamera->draw();
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
    /*  Initialisation for random number generation */
    std::random_device rd;
    std::mt19937 gen(rd());
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


  // // HUD elements for testing
  std::shared_ptr<AbstractHudElement> hudElement1(new ClickableHudElement(5, 5, 100, 50, hudTestFn1, ALIGN_CENTER, ALIGN_BOTTOM));
  mHudElements.push_back(hudElement1);
  std::shared_ptr<AbstractHudElement> hudElement2(new ClickableHudElement(5, 5, 100, 50, hudTestFn2, ALIGN_LEFT, ALIGN_BOTTOM));
  mHudElements.push_back(hudElement2);
  std::shared_ptr<AbstractHudElement> hudElement3(new ClickableHudElement(5, 5, 100, 50, hudTestFn3, ALIGN_RIGHT, ALIGN_BOTTOM));
  mHudElements.push_back(hudElement3);


  return;
}

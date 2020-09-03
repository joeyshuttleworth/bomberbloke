#include "BomberBlokeScene.hpp"
#include <random>
#include <memory>
#include <algorithm>
#include "engine.hpp"
#include "scene.hpp"
#include "bomberbloke.h"
#include "ClickableHudElement.hpp"
#include "TextHudElement.hpp"
#include "TextButton.hpp"
#include "SpriteHudElement.hpp"

static void hudTestFnJoin() {
  handle_system_command({"open", "localhost"});
}

static void hudTestFn1() {
  handle_system_command({"nickname", "dave1"});
}

static void hudTestFn2() {
  handle_system_command({"nickname",  "dave2"});
}

void BomberBlokeScene::draw(){
  if(!mpCamera){
    return;
    log_message(ERROR, "null camera");
  }

  mpCamera->resetFrameBuffer();

  int zoom = mpCamera->GetZoom();

  SDL_SetRenderTarget(_renderer, mpCamera->getFrameBuffer());
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


void BomberBlokeScene::logicUpdate(){
  // count blokes

  /*   */
  if(_tick % 30){

  }

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

  /* Create HUD elements */

  std::shared_ptr<Text> pTextTitle = textManager.createText("Aileron-Black", "BLOKE/ENGINE");
  pTextTitle->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
  pTextTitle->setTextColour({255, 255, 255});
  pTextTitle->setTextScale(2.);
  std::shared_ptr<TextHudElement> hudElementTitle = std::make_shared<TextHudElement>(pTextTitle, 5, 5, 400, 50, ALIGN_CENTER);
  hudElementTitle->setIsPostProcessed(false);
  mHudElements.push_back(hudElementTitle);

  std::shared_ptr<Text> pText1 = textManager.createText("Aileron-Black", "DAVE1");
  pText1->setTextAlignment(TEXT_ALIGN_LEFT, TEXT_ALIGN_CENTER);
  pText1->setTextColour({255, 255, 255});
  pText1->setTextScale(1.5);
  std::shared_ptr<TextButton> hudElement1 = std::make_shared<TextButton>(pText1, 9, 71, 200, 30, hudTestFn1, ALIGN_LEFT, ALIGN_BOTTOM);
  hudElement1->setMouseOverColour({200, 200, 200});
  hudElement1->setOnClickOffset(-1, 2);
  mHudElements.push_back(hudElement1);

  std::shared_ptr<Text> pText2 = textManager.createText("Aileron-Black", "DAVE2");
  pText2->setTextAlignment(TEXT_ALIGN_LEFT, TEXT_ALIGN_CENTER);
  pText2->setTextColour({255, 255, 255});
  pText2->setTextScale(1.5);
  std::shared_ptr<TextButton> hudElement2 = std::make_shared<TextButton>(pText2, 9, 40, 200, 30, hudTestFn2, ALIGN_LEFT, ALIGN_BOTTOM);
  hudElement2->setMouseOverColour({200, 200, 200});
  hudElement2->setOnClickOffset(-1, 2);
  mHudElements.push_back(hudElement2);

  std::shared_ptr<Text> pTextJoin = textManager.createText("Aileron-Black", "JOIN LOCAL");
  pTextJoin->setTextAlignment(TEXT_ALIGN_LEFT, TEXT_ALIGN_CENTER);
  pTextJoin->setTextColour({255, 255, 255});
  pTextJoin->setTextScale(1.5);
  std::shared_ptr<TextButton> hudElementJoin = std::make_shared<TextButton>(pTextJoin, 9, 9, 200, 30, hudTestFnJoin, ALIGN_LEFT, ALIGN_BOTTOM);
  hudElementJoin->setMouseOverColour({200, 200, 200});
  hudElementJoin->setOnClickOffset(-1, 2);
  mHudElements.push_back(hudElementJoin);

  // Speed HUD demo
  for (int i = 0; i < 4; i++) {
    std::shared_ptr<SpriteHudElement> hudElement = std::make_shared<SpriteHudElement>("lightning.png", 9 + i * 34, 9, 32, 32);
    hudElement->setGlowAmount(150);
    mSpeedIcons[i] = hudElement;
    mHudElements.push_back(hudElement);
  }

  // Power HUD demo
  for (int i = 0; i < 3; i++) {
    std::shared_ptr<SpriteHudElement> hudElement = std::make_shared<SpriteHudElement>("flames.png", 9 + i * 34, 9, 32, 32, ALIGN_RIGHT);
    hudElement->setGlowAmount(150);
    mPowerIcons[i] = hudElement;
    mHudElements.push_back(hudElement);
  }

  return;
}

void BomberBlokeScene::onInput(SDL_Event *event) {
    scene::onInput(event);

    // Blur effect demo
    if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_ESCAPE) {
        mpCamera->blur(10);
    } else if (event->type == SDL_KEYUP && event->key.keysym.sym == SDLK_ESCAPE) {
        mpCamera->blur(0);
    }
}

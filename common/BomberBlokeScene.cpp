#include "BomberBlokeScene.hpp"
#include <random>
#include <memory>
#include <algorithm>
#include <string>
#include "engine.hpp"
#include "scene.hpp"
#include "bomberbloke.h"
#include "ClickableHudElement.hpp"
#include "TextHudElement.hpp"
#include "TextButton.hpp"
#include "SpriteHudElement.hpp"
#include "FollowCamera.hpp"
#include "woodenCrate.hpp"
#include "bloke.hpp"
#include "PauseMenuHudGroup.hpp"

const std::string BACKGROUND_TILE_PREFIX = "background_tile_";
const int N_BACKGROUND_TILES = 10;

const int PAUSE_BLUR_SIZE = 10;
const int PAUSE_BRIGHTNESS = -30;

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
  // Reset the frame buffer
  mpCamera->resetFrameBuffer();

  // Draw background
  SDL_Rect sceneScreenRect = mpCamera->getScreenRect(0, 0, mDimmension[0], mDimmension[1]);
  mpCamera->renderCopy(mBackgroundTexture, nullptr, &sceneScreenRect);

  // Draw actors, particles and HUD
  drawActors();
  drawParticles();
  drawHud();
  // Draw camera to window
  mpCamera->draw();
}

void BomberBlokeScene::update() {
  // Check if blokeCamera doesn't have a subject
  if (blokeCamera->mSubject.expired()) {
    if (_local_player_list.size() > 0 && _local_player_list.back().getCharacter()) {
      // Get player actor and use blokeCamera
      blokeCamera->mSubject = _local_player_list.back().getCharacter();
      mpCamera = blokeCamera;
    }
    // TODO: go back to full scene camera when the player is removed
  }

  // Update scene
  scene::update();
}

void BomberBlokeScene::logicUpdate(){
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
  /*  Initialisation for random number generation */
  std::random_device rd;
  std::mt19937 gen(rd());


 if(_server){
    /*  Initialisation for random number generation */
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


  // Create cameras
  blokeCamera = std::make_shared<FollowCamera>(this);
  blokeCamera->mZoom = 0.1;
  mpCamera->mPosition[0] = ((double) size_x) / 2;
  mpCamera->mPosition[1] = ((double) size_y) / 2;
  mpCamera->mZoom = 1 / std::fmax(size_x, size_y);

  /* Create tiled background texture */
  mBackgroundTexture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, size_x * 64, size_y * 64);
  SDL_SetRenderTarget(_renderer, mBackgroundTexture);

  SDL_Texture *tileTexture;
  SDL_Rect tileRect({0, 0, 64, 64});
  std::uniform_int_distribution<> tileDistribution(0, N_BACKGROUND_TILES - 1);
  std::uniform_int_distribution<> flipDistribution(0, 1);
  for (int i = 0; i < size_x; i++) {
      for (int j = 0; j < size_y; j++) {
          tileRect.x = i * 64;
          tileRect.y = j * 64;
          // Randomly choose tile
          int tileIndex = tileDistribution(gen);
          tileTexture = get_sprite(BACKGROUND_TILE_PREFIX + std::to_string(tileIndex) + ".png");
          // Randomly choose whether to flip the texture
          if (flipDistribution(gen) == 1)
            // Flip and rotate in such a way that the top-right corner is static
            SDL_RenderCopyEx(_renderer, tileTexture, nullptr, &tileRect, 90,
              nullptr, SDL_FLIP_HORIZONTAL);
          else
            // Don't flip the texture
            SDL_RenderCopy(_renderer, tileTexture, nullptr, &tileRect);
      }
  }
  SDL_SetRenderTarget(_renderer, mpCamera->getFrameBuffer());

  /* Create HUD elements */

  std::shared_ptr<Text> pText1 = textManager.createText("Aileron-Black", "DAVE1");
  pText1->setTextAlignment(TEXT_ALIGN_LEFT, TEXT_ALIGN_CENTER);
  pText1->setTextColour({255, 255, 255});
  pText1->setTextScale(1.5);
  std::shared_ptr<TextButton> hudElement1 = std::make_shared<TextButton>(pText1, 9, -71, 200, 30, hudTestFn1, ALIGN_LEFT, ALIGN_BOTTOM);
  hudElement1->setMouseOverColour({200, 200, 200});
  hudElement1->setOnClickOffset(-1, 2);
  mHudElements.push_back(hudElement1);

  std::shared_ptr<Text> pText2 = textManager.createText("Aileron-Black", "DAVE2");
  pText2->setTextAlignment(TEXT_ALIGN_LEFT, TEXT_ALIGN_CENTER);
  pText2->setTextColour({255, 255, 255});
  pText2->setTextScale(1.5);
  std::shared_ptr<TextButton> hudElement2 = std::make_shared<TextButton>(pText2, 9, -40, 200, 30, hudTestFn2, ALIGN_LEFT, ALIGN_BOTTOM);
  hudElement2->setMouseOverColour({200, 200, 200});
  hudElement2->setOnClickOffset(-1, 2);
  mHudElements.push_back(hudElement2);

  std::shared_ptr<Text> pTextJoin = textManager.createText("Aileron-Black", "JOIN LOCAL");
  pTextJoin->setTextAlignment(TEXT_ALIGN_LEFT, TEXT_ALIGN_CENTER);
  pTextJoin->setTextColour({255, 255, 255});
  pTextJoin->setTextScale(1.5);
  std::shared_ptr<TextButton> hudElementJoin = std::make_shared<TextButton>(pTextJoin, 9, -9, 200, 30, hudTestFnJoin, ALIGN_LEFT, ALIGN_BOTTOM);
  hudElementJoin->setMouseOverColour({200, 200, 200});
  hudElementJoin->setOnClickOffset(-1, 2);
  mHudElements.push_back(hudElementJoin);

  std::shared_ptr<PauseMenuHudGroup> pPauseMenu = std::make_shared<PauseMenuHudGroup>();
  pPauseMenu->setIsVisible(false);
  pPauseMenu->mIsInteractive = false;
  mHudElements.push_back(pPauseMenu);
  mPauseMenuHud = pPauseMenu;

  // Speed HUD demo
  for (int i = 0; i < 4; i++) {
    std::shared_ptr<SpriteHudElement> hudElement = std::make_shared<SpriteHudElement>("lightning.png", 9 + i * 34, 9, 32, 32);
    hudElement->setGlowAmount(100);
    mSpeedIcons[i] = hudElement;
    mHudElements.push_back(hudElement);
  }

  // Power HUD demo
  for (int i = 0; i < 3; i++) {
    std::shared_ptr<SpriteHudElement> hudElement = std::make_shared<SpriteHudElement>("flames.png", -(9 + i * 34), 9, 32, 32, ALIGN_RIGHT);
    hudElement->setGlowAmount(100);
    mPowerIcons[i] = hudElement;
    mHudElements.push_back(hudElement);
  }

  return;
}

void BomberBlokeScene::onInput(SDL_Event *event) {
  // TODO: Consider disabling player control when paused
  if (!mIsPaused) {
    scene::onInput(event);
  } else {
    // Is paused so only share input with pause menu
    std::shared_ptr<PauseMenuHudGroup> pPauseMenu = mPauseMenuHud.lock();
    pPauseMenu->onInput(event);
  }


  // Toggle pause
  if (event->type == SDL_KEYUP && event->key.keysym.sym == SDLK_ESCAPE)
    togglePause();
}

void BomberBlokeScene::togglePause() {
  if (!mIsPaused) {
    // Pause game
    mIsPaused = true;

    // Set post-processing effects
    mpCamera->setBlur(PAUSE_BLUR_SIZE);
    mpCamera->setBrightness(PAUSE_BRIGHTNESS);

    // Make pause menu visible and interactive
    std::shared_ptr<PauseMenuHudGroup> pPauseMenu = mPauseMenuHud.lock();
    pPauseMenu->setIsVisible(true);
    pPauseMenu->mIsInteractive = true;
  } else {
    // Un-pause game
    mIsPaused = false;

    // Reset post-processing effects
    mpCamera->setBlur(0);
    mpCamera->setBrightness(0);

    // Make pause menu invisible and non-interactive
    std::shared_ptr<PauseMenuHudGroup> pPauseMenu = mPauseMenuHud.lock();
    pPauseMenu->setIsVisible(false);
    pPauseMenu->mIsInteractive = false;
  }
}

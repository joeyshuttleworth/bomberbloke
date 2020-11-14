#include "BomberBlokeScene.hpp"
#include "bomberbloke.h"
#include <random>
#include <memory>
#include <algorithm>
#include <functional>
#include <string>
#include "CommandEvent.hpp"
#include "engine.hpp"
#include "scene.hpp"
#include "ClickableHudElement.hpp"
#include "CountdownHudGroup.hpp"
#include "TextHudElement.hpp"
#include "TextButton.hpp"
#include "SpriteHudElement.hpp"
#include "FollowCamera.hpp"
#include "woodenCrate.hpp"
#include "bloke.hpp"
#include "EndRoundHudGroup.hpp"
#include "PauseMenuHudGroup.hpp"
#include "Soundtrack.hpp"
#include "syncEvent.hpp"

const std::string BACKGROUND_TILE_PREFIX = "background_tile_";
const int N_BACKGROUND_TILES = 10;

const int PAUSE_BLUR_SIZE = 10;
const int PAUSE_BRIGHTNESS = -30;

const int ROUND_END_WAIT_SECS = 5;

void BomberBlokeScene::setBigBomb(){
  std::shared_ptr<AbstractHudElement> observe = mBombIcons[0].lock();
  mHudElements.remove(observe);
  std::shared_ptr<SpriteHudElement> hudElement = std::make_shared<SpriteHudElement>("bigredbomb.png", 9 + 0 * 34, 91, 32, 32);
  hudElement->setGlowAmount(100);
  mBombIcons[0] = hudElement;
  mHudElements.push_back(hudElement);
}

BomberBlokeScene::~BomberBlokeScene() {
  mNewGame = false;
  if (mSoundtrack)
    mSoundtrack->stop();
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

  /* Set the HUD icons to be visible based on our player properties */

  //Power
  for(unsigned int i = 0; i < 10; i++){
    if(auto observe = mPowerIcons[i].lock()){
      if(i < _local_player_list.back().getPlayerProperties()->mPower)
        observe->setIsVisible(true);
      else
        observe->setIsVisible(false);
    }
  }

  //Speed
  for(unsigned int i = 0; i < 10; i++){
    if(auto observe = mSpeedIcons[i].lock()){
      if(i < _local_player_list.back().getPlayerProperties()->mSpeed)
        observe->setIsVisible(true);
      else
        observe->setIsVisible(false);
    }
  }

  //Bombs
  for(unsigned int i = 0; i < 10; i++){
    if(auto observe = mBombIcons[i].lock()){
      if(i < _local_player_list.back().getPlayerProperties()->mMaxBombs)
        observe->setIsVisible(true);
      else
        observe->setIsVisible(false);
    }
  }

  drawHud();

  // Draw camera to window
  mpCamera->draw();
}

void BomberBlokeScene::update() {
  // Check if blokeCamera doesn't have a subject
  if (!mIsFollowingBloke || mBlokeCamera->mSubject.expired()) {
    if (_local_player_list.size() > 0 && _local_player_list.back().getCharacter()) {
      // Get player actor and use bloke camera
      followBloke(_local_player_list.back().getCharacter());
    }
  }

  // Update scene
  scene::update();
}

void BomberBlokeScene::logicUpdate(){
  if(!_server)
    return;

  // count blokes
  if(mState == PAUSED || mState == STOPPED)
    return;

  int number_of_blokes = std::count_if(mActors.begin(), mActors.end(), [](std::shared_ptr<actor> i) -> bool {return i->getType() == ACTOR_BLOKE;});
  if(!mIsRoundEnd && !mNewGame){
    if(number_of_blokes <= 1){
      /*Send end command*/
      mIsRoundEnd = true;
      mRoundEndTicks = 0;
      /* Find the last player left alive that is, the winner of this round.
         winner_iter == _player_list.end() if no winner exists.
       */
      auto winner_iter = std::find_if(_player_list.begin(), _player_list.end(), [&](std::shared_ptr<AbstractPlayer> p) -> bool {return p->getCharacter() != nullptr;});
      std::unique_ptr<AbstractEvent> c_event(new CommandEvent("end nobody"));
      if(winner_iter!=_player_list.end())
        c_event = std::unique_ptr<AbstractEvent>(new CommandEvent("end " + (*winner_iter)->mNickname));
      {
        std::unique_ptr<syncEvent> s_event(new syncEvent());
        cereal::JSONOutputArchive outArchive(std::cout);
        outArchive(s_event);
      }
        _net_server.broadcastEvent(c_event);
    }
  } else if (mIsRoundEnd) {
    mRoundEndTicks++;
    if (mRoundEndTicks > ROUND_END_WAIT_SECS * TICK_RATE) {
      mNewGame = true;
    }
  }

  if (mSoundtrack) {
    double intensity = 1 - ((double) number_of_blokes) / ((double) _player_list.size());
    mSoundtrack->setIntensity(intensity);
  }
}

BomberBlokeScene::BomberBlokeScene(int size_x, int size_y) : scene(size_x, size_y){
  mState = STOPPED;
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
        log_message(ERR, "failed to generate spawn points");
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
      addActor(std::make_shared<bloke>(spawn_points[i][0], spawn_points[i][1], true));
      linkActorToPlayer(mActors.back(), (*iter)->getId());
      iter++;
    }
  }

  log_message(INFO, "no. actors " + std::to_string(mActors.size()));

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

  std::shared_ptr<PauseMenuHudGroup> pPauseMenu = std::make_shared<PauseMenuHudGroup>();
  pPauseMenu->setIsVisible(false);
  pPauseMenu->mIsInteractive = false;
  mHudElements.push_back(pPauseMenu);
  mPauseMenuHud = pPauseMenu;

  auto countdownFn = std::bind(&BomberBlokeScene::onCountdownFinished, this);
  std::shared_ptr<CountdownHudGroup> pCountdown = std::make_shared<CountdownHudGroup>(countdownFn, 150);
  mHudElements.push_back(pCountdown);
  mCountdownHud = pCountdown;

  std::shared_ptr<EndRoundHudGroup> endRoundHud = std::make_shared<EndRoundHudGroup>();
  endRoundHud->setIsVisible(false);
  mHudElements.push_back(endRoundHud);
  mEndRoundHud = endRoundHud;

  // Speed HUD demo
  for(int i = 0; i < 10; i++) {
    std::shared_ptr<SpriteHudElement> hudElement = std::make_shared<SpriteHudElement>("lightning.png", 9 + i * 34, 9, 32, 32);
    hudElement->setGlowAmount(100);
    mSpeedIcons[i] = hudElement;
    mHudElements.push_back(hudElement);
  }

  // Power HUD demo
  for(int i = 0; i < 10; i++) {
    std::shared_ptr<SpriteHudElement> hudElement = std::make_shared<SpriteHudElement>("flames.png", 9 + i * 34, 43, 32, 32);
    hudElement->setGlowAmount(100);
    mPowerIcons[i] = hudElement;
    mHudElements.push_back(hudElement);
  }

  // Bomb HUD demo
  for(int i = 0; i < 10; i++) {
    std::shared_ptr<SpriteHudElement> hudElement = std::make_shared<SpriteHudElement>("bomb_pickup.png", 9 + i * 34, 91, 32, 32);
    hudElement->setGlowAmount(100);
    mBombIcons[i] = hudElement;
    mHudElements.push_back(hudElement);
  }

  // Create bloke camera
  mSceneCamera = mpCamera;
  mBlokeCamera = std::make_shared<FollowCamera>(this);

  showEntireScene();

  if(_server)
    startCountdown(5);
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

void BomberBlokeScene::followBloke(std::shared_ptr<actor> subject) {
  mBlokeCamera->mSubject = subject;
  mBlokeCamera->mZoom = 0.1;

  mpCamera = mBlokeCamera;
  mIsFollowingBloke = true;
}

void BomberBlokeScene::showEntireScene() {
  mSceneCamera->mPosition[0] = ((double) mDimmension[0]) / 2;
  mSceneCamera->mPosition[1] = ((double) mDimmension[1]) / 2;
  mSceneCamera->mZoom = 1 / std::fmax(mDimmension[0], mDimmension[1]);

  mpCamera = mSceneCamera;
  mIsFollowingBloke = false;
}

void BomberBlokeScene::togglePause() {
  if (!mIsPaused) {
    // Pause game
    mIsPaused = true;

    // Set post-processing effects
    mBlokeCamera->setBlur(PAUSE_BLUR_SIZE);
    mBlokeCamera->setBrightness(PAUSE_BRIGHTNESS);
    mSceneCamera->setBlur(PAUSE_BLUR_SIZE);
    mSceneCamera->setBrightness(PAUSE_BRIGHTNESS);

    // Make pause menu visible and interactive
    std::shared_ptr<PauseMenuHudGroup> pPauseMenu = mPauseMenuHud.lock();
    pPauseMenu->setIsVisible(true);
    pPauseMenu->mIsInteractive = true;
  } else {
    // Un-pause game
    mIsPaused = false;

    // Reset post-processing effects
    mBlokeCamera->setBlur(0);
    mBlokeCamera->setBrightness(0);
    mSceneCamera->setBlur(0);
    mSceneCamera->setBrightness(0);

    // Make pause menu invisible and non-interactive
    std::shared_ptr<PauseMenuHudGroup> pPauseMenu = mPauseMenuHud.lock();
    pPauseMenu->setIsVisible(false);
    pPauseMenu->mIsInteractive = false;
  }
}

void BomberBlokeScene::startCountdown(int nSecs) {
  std::shared_ptr<CountdownHudGroup> countdown = mCountdownHud.lock();
  countdown->start(nSecs);
  if(_server){
    std::unique_ptr<AbstractEvent> c_event(new CommandEvent("start"));
    _net_server.broadcastEvent(c_event);
  }
}

void BomberBlokeScene::onCountdownFinished() {
  mState = PLAYING;
  if(_server){
    _net_server.syncPlayers();
  }
  if (mSoundtrack)
    mSoundtrack->play();
}

void BomberBlokeScene::handleCommand(std::string str){
  auto tokens = split_to_tokens(str);
  if (str == "start"){
    /*  reset big bomb sprite */
    std::shared_ptr<AbstractHudElement> observe = mBombIcons[0].lock();
    mHudElements.remove(observe);
    std::shared_ptr<SpriteHudElement> hudElement = std::make_shared<SpriteHudElement>("bomb_pickup.png", 9 + 0 * 34, 91, 32, 32);
    hudElement->setGlowAmount(100);
    mBombIcons[0] = hudElement;
    if (mSoundtrack)
      mSoundtrack->stop();

    // TODO: make this work for more than two tracks
    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution dist;
    if (dist(gen)) {
      mSoundtrack = std::make_shared<Soundtrack1>();
    } else {
      mSoundtrack = std::make_shared<Soundtrack2>();
    }
    mSoundtrack->playIdle();

    std::shared_ptr<EndRoundHudGroup> endRoundHud = mEndRoundHud.lock();
    endRoundHud->setIsVisible(false);

    startCountdown(5);
  } else if(tokens.front() == "end") {
    mIsRoundEnd = true;
    mRoundEndTicks = 0;
    mState = STOPPED;

    if (mSoundtrack)
      mSoundtrack->playIdle();

    std::string winning_name = tokens.back();
    std::shared_ptr<EndRoundHudGroup> endRoundHud = mEndRoundHud.lock();
    endRoundHud->updateScores(winning_name, _player_list);
    endRoundHud->setIsVisible(true);
  }
  if(str == "bigbomb")
    setBigBomb();
}

void BomberBlokeScene::onResize() {
  if(mSceneCamera)
    mSceneCamera->onResize();
  if(mBlokeCamera)
    mBlokeCamera->onResize();
  scene::onResize();
}

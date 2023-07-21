#ifndef BOMBERBLOKESCENE_HPP
#define BOMBERBLOKESCENE_HPP
#include "engine.hpp"
#include "scene.hpp"
#include "bomberbloke.h"
#include "ShowAllCamera.hpp"
#include <memory>


class FollowCamera;
class PauseMenuHudGroup;
class CountdownHudGroup;
class EndRoundHudGroup;
class TextHudElement;
class actor;
class Soundtrack;

class BomberBlokeScene : public scene{
protected:
  std::weak_ptr<AbstractHudElement> mPowerIcons[10];
  std::weak_ptr<AbstractHudElement> mSpeedIcons[10];
  std::weak_ptr<AbstractHudElement> mBombIcons[10];

  // Texture containing bakground tiles.
  SDL_Texture *mBackgroundTexture;

  // Camera that captures entire scene.
  std::shared_ptr<ShowAllCamera> mSceneCamera;
  // Camera that follows blokes.
  std::shared_ptr<FollowCamera> mBlokeCamera;
  // Boolean that is true when the camera is following a bloke.
  bool mIsFollowingBloke = false;

  // When true, the game is paused.
  bool mIsPaused = false;
  // Weak pointer to the pause menu HUD group (contained in mHudElements).
  std::weak_ptr<PauseMenuHudGroup> mPauseMenuHud;

  std::weak_ptr<CountdownHudGroup> mCountdownHud;

  bool mIsRoundEnd = false;
  int mRoundEndTicks = 0;
  // Weak pointer to the options menu.
  std::weak_ptr<EndRoundHudGroup> mEndRoundHud;

  // Soundtrack object.
  std::shared_ptr<Soundtrack> mSoundtrack;

  /**
   * Configures the camera such that it follows a bloke.
   *
   * @param subject Target bloke the camera follows.
   */
  void followBloke(std::shared_ptr<actor> subject);

  /**
   * Configures the camera such that the entire scene is visible.
   */
  void showEntireScene();

public:
  void setBigBomb();

  BomberBlokeScene(unsigned int size_x = 10, unsigned int size_y = 10);
  ~BomberBlokeScene();

  /**
   * Update function - called every tick
   */
  void update() override;

 /**
   * Every second, check how many players are alive.
   * If no players remain, everybody draws. Otherwise,
   * if one player remains, this player wins the match.
   *
   */
  void logicUpdate() override;

  /**
   * Draw function.
   */
  void draw() override;

  /**
   * Handles input.
   *
   * Primarily for interactive HUD elements.
   */
  void onInput(SDL_Event *event) override;

  /**
   * Alternates pause state.
   */
  void togglePause();

  /**
   * Starts the countdown (see mCountdownHud).
   *
   * @param nSecs Number of seconds counted down.
   */
  void startCountdown(int nSecs);

  /**
   * Callback for when the countdown is finished.
   */
  void onCountdownFinished();

  /**
   * Handles server command.
   *
   * @param str Server command to handle.
   */
  void handleCommand(std::string str) override;

  /**  onResize
   *
   *  Update the camera to reflect the new window size.
   */
  void onResize() override;

  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::base_class<scene>(this));
    return;
  }
};

CEREAL_REGISTER_TYPE(BomberBlokeScene)
#endif

#ifndef BOMBERBLOKESCENE_HPP
#define BOMBERBLOKESCENE_HPP
#include "engine.hpp"
#include "scene.hpp"
#include "bomberbloke.h"

#include <memory>

class FollowCamera;
class PauseMenuHudGroup;
class TextHudElement;
class actor;

class BomberBlokeScene : public scene{
protected:
  std::weak_ptr<AbstractHudElement> mPowerIcons[10];
  std::weak_ptr<AbstractHudElement> mSpeedIcons[10];
  std::weak_ptr<AbstractHudElement> mBombIcons[10];

  // Texture containing bakground tiles.
  SDL_Texture *mBackgroundTexture;

  // Camera that captures entire scene.
  std::shared_ptr<Camera> mSceneCamera;
  // Camera that follows blokes.
  std::shared_ptr<FollowCamera> mBlokeCamera;
  // Boolean that is true when the camera is following a bloke.
  bool mIsFollowingBloke = false;

  // When true, the game is paused.
  bool mIsPaused = false;
  // Weak pointer to the pause menu HUD group (contained in mHudElements).
  std::weak_ptr<PauseMenuHudGroup> mPauseMenuHud;

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

  BomberBlokeScene(int size_x = 10, int size_y = 10);
  ~BomberBlokeScene(){}

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

  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::base_class<scene>(this));
    return;
  }
};

CEREAL_REGISTER_TYPE(BomberBlokeScene)
#endif

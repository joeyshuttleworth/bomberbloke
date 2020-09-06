#ifndef BOMBERBLOKESCENE_HPP
#define BOMBERBLOKESCENE_HPP
#include "ClickableHudElement.hpp"
#include "engine.hpp"
#include "scene.hpp"
#include "bomberbloke.h"

class FollowCamera;
class PauseMenuHudGroup;

class BomberBlokeScene : public scene{
protected:
  std::weak_ptr<AbstractHudElement> mPowerIcons[5];
  std::weak_ptr<AbstractHudElement> mSpeedIcons[5];

  // Texture containing bakground tiles.
  SDL_Texture *mBackgroundTexture;

  // Camera that follows local player.
  std::shared_ptr<FollowCamera> blokeCamera;

  // When true, the game is paused.
  bool mIsPaused = false;
  // Weak pointer to the pause menu HUD group (contained in mHudElements).
  std::weak_ptr<PauseMenuHudGroup> mPauseMenuHud;

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

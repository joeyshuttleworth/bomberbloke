#ifndef EXPLOSION_HPP
#define EXPLOSION_HPP

#include "AbstractSpriteHandler.hpp"
#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>
#include "engine.hpp"
#include "IGraphicsManager.hpp"

class Sound;

#define SPRITE_EXPLOSION 100
#define N_EXPLOSION_SOUNDS 2
#define N_SPRITESHEET_SIZE 6


class Explosion : public AbstractSpriteHandler{
protected:
  int mMaxGlowAmount;
  bool mStarted = false;
  bool mSound = true;
  bool mRumble = true;
  bool mRenderLegacy = false;
  std::array<std::shared_ptr<Sound>, N_EXPLOSION_SOUNDS> mExplosionSounds;
  const std::array<std::string, N_SPRITESHEET_SIZE> mSpriteNames = {
    "explosion_frame_1.png",
    "explosion_frame_2.png",
    "explosion_frame_3.png",
    "explosion_frame_4.png",
    "explosion_frame_5.png",
    "explosion_frame_6.png",
  };
  const std::string mExplosionSoundNames[N_EXPLOSION_SOUNDS] = {"bomb_1", "bomb_2"};
  void draw_legacy(Camera* cam);

public:
  int getType() const{
    return SPRITE_EXPLOSION;
  }

  Explosion(IGraphicsManager* = nullptr);

  /*  Use the default constructor for everything except creating the texture */
  Explosion(IGraphicsManager* gfx_manager, double x_pos, double y_pos, double x_dim, double y_dim, bool legacy, int speed = 30, int timeout = 64, int start_delay = 0, bool sound_on = true, bool rumble_on = true, int max_glow=255)
    :AbstractSpriteHandler(gfx_manager, x_pos, y_pos, x_dim, y_dim, speed, timeout, start_delay){
    mSound = sound_on;
    mRumble = rumble_on;
    mDelay = start_delay;
    mMaxGlowAmount = max_glow;
    mRenderLegacy = legacy;

    init();
  }

  void init(){
    initSounds();
  }

  void initSounds();

  /*  In draw() we cycle through the explosion sprites */
  void draw(Camera*);

  virtual ~Explosion(){}

  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::base_class<AbstractSpriteHandler>(this), mSound, mRumble, mRenderLegacy);
    return;
  }
};

CEREAL_REGISTER_TYPE(Explosion)

#endif

#ifndef EXPLOSION_HPP
#define EXPLOSION_HPP

#include "AbstractSpriteHandler.hpp"

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

  using AbstractSpriteHandler::AbstractSpriteHandler;

  Explosion(IGraphicsManager& gfx_manager, double x_pos=0, double y_pos=0, double x_dim=10, double y_dim=10, bool legacy=false, int speed = 30, int timeout = 64, int start_delay = 0, bool sound_on = true, bool rumble_on = true, int max_glow=255)
    : AbstractSpriteHandler(gfx_manager, x_pos, y_pos, x_dim, y_dim, speed, timeout, start_delay)
  {
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

  std::shared_ptr<AbstractSpriteHandler> clone(IGraphicsManager& gfx) override{
    return std::make_shared<Explosion>(*this, gfx);
  }

  ~Explosion(){}

  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::base_class<AbstractSpriteHandler>(this), mSound, mRumble, mRenderLegacy);
  }
};

CEREAL_REGISTER_TYPE(Explosion)

#endif

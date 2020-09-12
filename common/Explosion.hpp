#ifndef EXPLOSION_HPP
#define EXPLOSION_HPP

#include "AbstractSpriteHandler.hpp"
#include <cereal/cereal.hpp>

#define EXPLOSION_SPRITE_SIZE 1000
#define SPRITE_EXPLOSION 100
#define N_EXPLOSION_SOUNDS 2


class Explosion : public AbstractSpriteHandler{
protected:
  int mMaxGlowAmount;
  bool mStarted = false;
  bool mSound = true;
  bool mRumble = true;
  std::array<std::shared_ptr<Sound>, N_EXPLOSION_SOUNDS> mExplosionSounds;
  const std::string mExplosionSoundNames[N_EXPLOSION_SOUNDS] = {"bomb_1", "bomb_2"};
public:
  int getType() const{
    return SPRITE_EXPLOSION;
  }

  Explosion(){
    /* Create sound objects for explosion sound effects */
    for (int i = 0; i < N_EXPLOSION_SOUNDS; i++) {
      std::shared_ptr<Sound> sound = soundManager.createSound(mExplosionSoundNames[i]);
      mExplosionSounds[i] = sound;
    }
   return;
  }

  /*  Use the default constructor for everything except creating the texture */
  Explosion(double x_pos, double y_pos, double x_dim, double y_dim, int speed = 30, int timeout = TICK_RATE, int start_delay = 0, bool sound_on = true, bool rumble_on = true, int max_glow=255)
    :AbstractSpriteHandler(x_pos, y_pos, x_dim, y_dim, speed, timeout, start_delay){
    mSound = sound_on;
    mRumble = rumble_on;
    mDelay = start_delay;
    mMaxGlowAmount = max_glow;

    Explosion();
  }

  /*  In draw() we program a simple flash between red and white. */

  void draw(Camera*);

  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::base_class<AbstractSpriteHandler>(this), mSound);
    return;
  }
};


CEREAL_REGISTER_TYPE(Explosion)

#endif

#ifndef EXPLOSION_HPP
#define EXPLOSION_HPP

#include "AbstractSpriteHandler.hpp"
#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>

class Sound;

#define EXPLOSION_SPRITE_SIZE 1000
#define SPRITE_EXPLOSION 100
#define N_EXPLOSION_SOUNDS 2
#define N_SPRITESHEET_SIZE 6

SDL_Texture* get_sprite(std::string);


class Explosion : public AbstractSpriteHandler{
protected:
  int mMaxGlowAmount;
  bool mStarted = false;
  bool mSound = true;
  bool mRumble = true;
  bool mRenderLegacy = false;
  std::array<std::shared_ptr<Sound>, N_EXPLOSION_SOUNDS> mExplosionSounds;
  std::array<SDL_Texture*, N_SPRITESHEET_SIZE> mSpritesheet;
  const std::string mExplosionSoundNames[N_EXPLOSION_SOUNDS] = {"bomb_1", "bomb_2"};
  void draw_legacy(Camera* cam);
public:
  int getType() const{
    return SPRITE_EXPLOSION;
  }

  Explosion();

  /*  Use the default constructor for everything except creating the texture */
  Explosion(double x_pos, double y_pos, double x_dim, double y_dim, bool legacy, int speed = 30, int timeout = 64, int start_delay = 0, bool sound_on = true, bool rumble_on = true, int max_glow=255)
    :AbstractSpriteHandler(x_pos, y_pos, x_dim, y_dim, speed, timeout, start_delay){
    mSound = sound_on;
    mRumble = rumble_on;
    mDelay = start_delay;
    mMaxGlowAmount = max_glow;
    mRenderLegacy = legacy;

    for(int i = 1; i <= N_SPRITESHEET_SIZE; i++)
      mSpritesheet[i] = get_sprite("explosion_frame_" + std::to_string(i) + ".png");

    Explosion();
  }

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

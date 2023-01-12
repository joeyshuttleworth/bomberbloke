#ifndef ABSTRACTSPRITEHANDLER_HPP
#define ABSTRACTSPRITEHANDLER_HPP
#include <SDL3/SDL.h>
#include <array>
#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>

enum SpriteType{
                SPRITE_PLACEHOLDER=1,
                SPRITE_STATIC
};

class Camera;

extern unsigned int _tick;

class AbstractSpriteHandler{
public:
  // TODO: move all flashing stuff to a subclass
  AbstractSpriteHandler(double x_pos=0, double y_pos=0, double x_dim=0, double y_dim=0, int speed = 300, int timeout = 0, int delay = 0){
    mPosition[0] = x_pos;
    mPosition[1] = y_pos;
    mDimmension[0] = x_dim;
    mDimmension[1] = y_dim;
    mStartTick = _tick + delay;
    mAnimationSpeed = speed;
    mTimeout = timeout;
    mRemove = false;
    mDelay = delay;
    return;
  }

  /**
   *  Draw the sprite to the framebuffer of the Camera
   *  @param the camera we are drawing to.
   */
  virtual void draw(Camera*)= 0;

  virtual void refreshSprite(){};

  /**
   * Should we remove this object from _particle_list?
   *
   * We should remove this object if its mRemove flag has been set or if the time its been
   * on the screen exceeds mTimeout
   *
   * @return A bool: true if the object is to be removed, false if not.
   *
   **/
  bool ToRemove(){
    if(!mInitialised)
      return false;
    if(mRemove)
      return true;
    if(_tick > mStartTick && _tick - mStartTick > mTimeout && mTimeout != 0){
      return true;
    }
    else
      return false;
  }

  virtual void update(std::array<double,2> coords){
    mPosition[0] = coords[0];
    mPosition[1] = coords[1];
    update();
    return;
  }

  virtual void update(){
    if(!mInitialised){
      mInitialised = true;
      mStartTick = _tick + mDelay;
    }
    return;
  }


  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::make_nvp("position", mPosition), cereal::make_nvp("dimmension", mDimmension), cereal::make_nvp("animation speed", mAnimationSpeed), cereal::make_nvp("timeout", mTimeout), cereal::make_nvp("delay", mDelay));
    return;
  }

protected:
  bool mInitialised = false;

protected:
  double mPosition[2];
  double mDimmension[2];
  unsigned int mStartTick;
  unsigned int mAnimationSpeed;
  unsigned int mTimeout;
  unsigned int mDelay;
  bool mRemove;
};

CEREAL_REGISTER_TYPE(AbstractSpriteHandler)
#endif

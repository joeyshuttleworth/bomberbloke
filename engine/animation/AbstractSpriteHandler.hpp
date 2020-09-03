#ifndef ABSTRACTSPRITEHANDLER_HPP
#define ABSTRACTSPRITEHANDLER_HPP
#include <SDL2/SDL.h>
#include <array>
#include <cereal/cereal.hpp>

enum SpriteType{
                SPRITE_PLACEHOLDER=1,
                SPRITE_STATIC
};

extern unsigned int _tick;
class Camera;
class AbstractSpriteHandler{
public:
  virtual int getType() const = 0;

  unsigned int getStartTick(){return mStartTick;}

  AbstractSpriteHandler(){}

  virtual ~AbstractSpriteHandler(){}

  virtual void draw(Camera*) = 0;

  AbstractSpriteHandler(double x_pos, double y_pos, double x_dim, double y_dim, int speed = 300, int timeout = 0, unsigned int delay = 0){
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
  bool mInitialised;

protected:
  double mPosition[2];
  double mDimmension[2];
  unsigned int mStartTick;
  unsigned int mAnimationSpeed;
  unsigned int mTimeout;
  unsigned int mDelay;
  bool mRemove;
};

#endif

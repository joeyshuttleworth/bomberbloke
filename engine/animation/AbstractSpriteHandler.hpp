#ifndef ABSTRACTSPRITEHANDLER_HPP
#define ABSTRACTSPRITEHANDLER_HPP
#include <array>
#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>

#include "DummyGraphicsManager.hpp"
#include "IGraphicsManager.hpp"
#include "Camera.hpp"

enum SpriteType{
                SPRITE_PLACEHOLDER=1,
                SPRITE_STATIC
};

class Camera;

extern unsigned int _tick;

using d_vector = std::array<double, 2>;

class IGraphicsManager;

class AbstractSpriteHandler{

protected:
  IGraphicsManager& mrGraphicsManager;
  bool mInitialised = false;
  dvector mPosition = {0, 0};
  dvector mDimension = {10, 10};
  unsigned int mStartTick = 0;
  unsigned int mAnimationSpeed = 0;
  unsigned int mTimeout = 0;
  unsigned int mDelay = 0;
  bool mRemove = false;
  bool mIsPostProcessed = true;

  /* Used only in the case that the object is created with the default constructor and isn't given a graphics manager */


public:
  AbstractSpriteHandler(IGraphicsManager& gfx, double x_pos=0, double y_pos=0, double x_dim=0, double y_dim=0, int speed = 300, int timeout = 0, int delay = 0) :
    mrGraphicsManager(gfx),
    mPosition{x_pos, y_pos},
    mDimension{x_dim, y_dim},
    mStartTick(_tick + delay),
    mAnimationSpeed(speed),
    mTimeout(timeout),
    mDelay(delay)
  {
  }

  AbstractSpriteHandler(const AbstractSpriteHandler& other) : AbstractSpriteHandler(){
    mrGraphicsManager = other.mrGraphicsManager;
    mPosition[0] = other.mPosition[0];
    mPosition[1] = other.mPosition[1];

    mDimension[0] = other.mDimension[0];
    mDimension[1] = other.mDimension[1];
    mStartTick = other.mStartTick;
    mAnimationSpeed = other.mAnimationSpeed;
    mTimeout = other.mTimeout;
    mDelay = other.mDelay;
    mRemove = other.mRemove;
    mIsPostProcessed = other.mIsPostProcessed;
  }

  AbstractSpriteHandler() :
    mrGraphicsManager(_fallback_IO_system.getGraphicsManager()){
  }

  AbstractSpriteHandler(IGraphicsManager& gfx) : mrGraphicsManager(gfx){
  }

  AbstractSpriteHandler(const AbstractSpriteHandler& other, IGraphicsManager& gfx) :
    AbstractSpriteHandler(other)
  {
    mrGraphicsManager = gfx;
  };

  std::shared_ptr<AbstractSpriteHandler> clone(){
    return clone(mrGraphicsManager);
  };

  virtual std::shared_ptr<AbstractSpriteHandler> clone(IGraphicsManager&){
    return nullptr;
  };

  /**
   *  Draw the sprite to the framebuffer of the Camera
   *  @param the camera we are drawing to.
   */
  virtual void draw(Camera*){}

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
  void serialize(Archive&){
    // archive(mPosition, mDimension, mAnimationSpeed, mTimeout, mDelay);
    // return;
  }
};

CEREAL_REGISTER_TYPE(AbstractSpriteHandler)
#endif

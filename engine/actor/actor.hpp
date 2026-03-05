#ifndef ACTOR_HPP
#define ACTOR_HPP

#include <SDL.h>
#include <memory>
#include <array>
#include "Interpolator.hpp"
#include "KinematicCollider.hpp"
#include "Camera.hpp"
#include "IGraphicsManager.hpp"
#include "AbstractSpriteHandler.hpp"
#include "scene.hpp"
#include <cereal/types/polymorphic.hpp>

class IGraphicsManager;
class AbstractPlayer; class AbstractSpriteHandler;

class actor: public KinematicCollider {
  friend class MoveEvent;
  friend class NetClient;
  friend class NetServer;
  friend class scene;
protected:

  /*Flag to indicate removal when next updated*/
  bool mRemove = false;

  bool mMoved=false;
  std::shared_ptr<AbstractSpriteHandler> mpSpriteHandler;

  /*Who does is this actor controlled by? This corresponds
    to a unique id of a player in _player_list. 0 corresponds
    to the server.
  */
  int mPlayerId = 0;

  virtual void init(){};

  void setPlayerId(int id){mPlayerId = id;}

  /*The id of this actor. Used by  scene::mActors*/
  int mId = -1;

  Interpolator mInterpolator;

  scene* mpScene = nullptr;

  void setScene(scene* scene){mpScene = scene;}

  IGraphicsManager* mpGraphicsManager = nullptr;

public:

  void setId(int id){mId = id;}

  void interpolate();

  /* TODO replace this */
  dvector mDimension;

  void draw(Camera *cam){
    if(mpSpriteHandler)
      mpSpriteHandler->draw(cam);
  }

  int getId(){
    return mId;
  }

  void addState(dvector position, dvector velocity, int tick){mInterpolator.addState(position, velocity, tick);}

  virtual ~actor(){}

  actor(scene* scene=nullptr, double x = 0, double y = 0, double xdim = DEFAULT_ACTOR_SIZE, double ydim = DEFAULT_ACTOR_SIZE, bool collides = true);

  /*Returns a pointer to the player object.
    This is found by searching _player_list
    if we haven't already*/
  std::shared_ptr<AbstractPlayer> getPlayer();

  int getPlayerId(){
    return mPlayerId;
  }

  /*Do we collide with other actors*/
  bool mCollides;

  void refreshSprite(){
    if(mpSpriteHandler)
      mpSpriteHandler->refreshSprite();
    return;
  }

  bool toRemove(){return mRemove;}

  void remove();

  void draw();
  int move(double x, double y);
  bool isMoving();
  dvector getMidpoint();
  virtual void update(){}

  void updateSprite(){
    if(mpSpriteHandler)
      mpSpriteHandler->update(mPosition);
  }

  virtual void handleCommand(std::string){}

  /*Serialise this class using cereal.*/

  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::make_nvp("actorId", mId),
            cereal::make_nvp("playerId", mPlayerId),
            mPosition[0], mPosition[1],
            mVelocity[0], mVelocity[1],
            mDimension[0], mDimension[1]
            );
  }

  IGraphicsManager* getGraphicsManager(){
    if(mpScene){
      return mpScene->getGraphicsManager();
    }
    return nullptr;
  };

};

CEREAL_REGISTER_TYPE(actor)

#endif

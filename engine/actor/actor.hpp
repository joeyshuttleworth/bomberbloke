#ifndef ACTOR_HPP
#define ACTOR_HPP

#include <SDL2/SDL.h>
#include <memory>
#include <array>
#include "Camera.hpp"
#include "Interpolator.hpp"
#include "KinematicCollider.hpp"

extern SDL_Renderer *_renderer;

class AbstractPlayer; class AbstractSpriteHandler;

class actor: public KinematicCollider {
  friend class MoveEvent;
protected:

  bool mMoved=false;
  std::shared_ptr<AbstractSpriteHandler> mpSpriteHandler;

  /* GetPlayer uses mPlayerId to return a pointer to the controlling player (if it exists)
     A value of 0 indicates that the actor is not controlled by any player.
  */


  /*Who does is this actor controlled by? This corresponds
    to a unique id of a player in _player_list. 0 corresponds
    to the server.
  */
  int    mPlayerId = 0;

  /*The id of this actor. Used by  scene::mActors*/
  unsigned int mId;

  Interpolator mInterpolator;

public:

  void setId(int id){mId = id;}

  void interpolate();

  /* TODO replace this */
  dvector mDimmension;

  void mInterpolate();

  void draw(Camera *cam){
    mpSpriteHandler->draw(cam);
  }

  int GetId(){
    return mId;
  }

  void addState(dvector position, dvector velocity, int tick){mInterpolator.addState(position, velocity, tick);}

  /*Flag to indicate removal when next updated*/
  bool mRemove = false;
  actor(double x = 0, double y = 0, double xdim = DEFAULT_ACTOR_SIZE, double ydim = DEFAULT_ACTOR_SIZE, bool collides = true);

  /*Returns an enum defined by the game identifying what type of actor this is
    e.g block, bloke.*/
  virtual int getType() const = 0;


  /*Returns a pointer to the player object.
    This is found by searching _player_list
    if we haven't already*/
  std::shared_ptr<AbstractPlayer> getPlayer();

  unsigned int getPlayerId(){
    return mPlayerId;
  }

  void setPlayerId(int id){mPlayerId = id;}

  /*Do we collide with other actors*/
  bool mCollides;

  void refreshSprite(){
    if(mpSpriteHandler)
      mpSpriteHandler->refreshSprite();
    return;
  }

  void draw();
  int move(double x, double y);
  bool isMoving();
  int init(double, double);
  dvector getMidpoint();
  virtual void update(){};

  void updateSprite(){
    mpSpriteHandler->update(mPosition);
  }

  virtual void handle_command(std::string){};

  /*Serialise this class using cereal.
    NB: We don't send the size of the actor (dimmension) as this should
    be handled by the properties stored in a child of this class. To see why,
    consider a game where the player's character model can only be one of two sizes,
    it seems silly to send a double[2] in this case.*/

  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::make_nvp("actorId", mId), mPlayerId, mPosition[0], mPosition[1], mVelocity[0], mVelocity[1]);
  }

};

CEREAL_REGISTER_TYPE(actor)
#endif

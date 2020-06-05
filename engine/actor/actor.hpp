#ifndef ACTOR_HPP
#define ACTOR_HPP

class player;
class actor{
  friend class MoveEvent;
protected:
  SDL_Texture *mpSprite = NULL;

  /* Defined by the game - used to keep track of what the actor is.
     For example, a bomb, block or player's character*/
  int mType;
  player *mpControllingPlayer = NULL;
  /* GetPlayer uses mPlayerId to return a pointer to the controlling player (if it exists)
     A value of 0 indicates that the actor is not controlled by any player.
  */
  int     mPlayerId;
  Uint32 mId;

public:
  /*Flag to indicate removal when next updated*/
  bool mRemove;
  void setController(player* p);
  actor(double, double);
  actor();

  /*Returns a pointer to the player object.
    This is found by searching _player_list
    if we haven't already*/
  player* getPlayer();
  
  /*Do we collide with other actors*/
  bool mCollides;

  
  /*Position is the bottom left hand side of the actor */  
  double mPosition[2];
  double mDimmension[2];
  double mVelocity[2];

 
  void draw();
  int move(double x, double y);
  bool is_moving();
  int init(double, double);
  double *get_midpoint();
  virtual void update() = 0;

  virtual void handle_command(std::string) = 0;

  /*Serialise this class using cereal.
    NB: We don't send the size of the actor (dimmension) as this should
    be handled by the properties stored in a child of this class. To see why,
    consider a game where the player's character model can only be one of two sizes,
    it seems silly to send a double[2] in this case.*/
  template<class Archive> 
  void serialize(Archive &archive){
    archive(mType, mId, mPlayerId, mPosition, mVelocity);
  };


};
#endif

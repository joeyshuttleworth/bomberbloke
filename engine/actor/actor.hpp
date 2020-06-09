#ifndef ACTOR_HPP
#define ACTOR_HPP

#include <SDL2/SDL.h>
#include <memory>

extern SDL_Renderer *_renderer;

class AbstractPlayer;
class actor{
  friend class MoveEvent;
protected:
  SDL_Texture *mpSprite = NULL;

  /* GetPlayer uses mPlayerId to return a pointer to the controlling player (if it exists)
     A value of 0 indicates that the actor is not controlled by any player.
  */


  /*Who does is this actor controlled by? This corresponds
    to a unique id of a player in _player_list. 0 corresponds
    to the server.
  */
  int    mPlayerId = 0;

  /*The id of this actor. Used by  _level.mActors*/
  Uint32 mId;

public:
  /*Flag to indicate removal when next updated*/

  int GetId(){
    return mId;
  }

  bool mRemove = false;
  actor(double, double);
  actor();
  /*Returns an enum defined by the game identifying what type of actor this is
    e.g block, bloke.*/
  
  virtual int getType() const = 0;


  /*Returns a pointer to the player object.
    This is found by searching _player_list
    if we haven't already*/
  std::shared_ptr<AbstractPlayer> getPlayer();
  
  /*Do we collide with other actors*/
  bool mCollides;

  
  /*Position is the bottom left hand side of the actor */  
  double mPosition[2];
  double mDimmension[2];
  double mVelocity[2];


  virtual void ReloadSprite(){
    if(mpSprite)
      SDL_DestroyTexture(mpSprite);
    mpSprite = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,1024, 1024);
    SDL_SetRenderTarget(_renderer, mpSprite);
    SDL_SetRenderDrawColor(_renderer, 0xFF, 0xA1, 0x0A, 0xFF);
    SDL_RenderClear(_renderer);
    SDL_RenderFillRect(_renderer, NULL);
    SDL_RenderPresent(_renderer);
    SDL_SetRenderTarget(_renderer, NULL);
    return;
  }

  void draw();
  int move(double x, double y);
  bool is_moving();
  int init(double, double);
  double get_midpoint(int);
  virtual void update() = 0;

  virtual void handle_command(std::string) = 0;

  /*Serialise this class using cereal.
    NB: We don't send the size of the actor (dimmension) as this should
    be handled by the properties stored in a child of this class. To see why,
    consider a game where the player's character model can only be one of two sizes,
    it seems silly to send a double[2] in this case.*/
  template<class Archive> 
  void serialize(Archive &archive){
    archive(mId, mPlayerId, mPosition, mVelocity);
    
  };

};

#endif

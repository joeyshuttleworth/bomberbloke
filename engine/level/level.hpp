#ifndef LEVEL_HPP
#define LEVEL_HPP

#include <list>
#include <string>
#include <SDL2/SDL.h>
#include <cereal/types/list.hpp>
#include <memory>

#include "AbstractSpriteHandler.hpp"


extern double _zoom;

class actor;

/* Actor which stores information about the level including the actors present and methods for updating and drawing the level */
class level{

protected:
  /*name and description are information about this level*/
  std::string mName;
  std::string mDescription;

public:
  /* Spawnpoints is a collection of coordinates where players can be spawned */ 
  std::vector<int> mSpawnPoints;

  /* Sprite holds the SDL_Texture which will be applied to the level background
     when draw is called */
  SDL_Texture *mpSprite = NULL;
/* dim_x and dim_y are the size of our level in the x and y axis respectively */
  double mDimmension[2];

  /*
   * mActors holds each object in our level.
   * Cereal will only work if we use smart pointers
   * here but we should probably be using them anyway *?
   *
   */
  std::list<std::shared_ptr<actor>> mActors;

  /*Draw our level on the window. Then draw every actor in mActors*/
  void draw();
  void ReloadSprites();

  /*
   * mParticleList lists our particles e.g explosions.
   * These won't be sent over the internet
   */

  std::list<std::shared_ptr<AbstractSpriteHandler>> mParticleList;


  level(double x, double y);
  level();
  level(const level &lvl);
  ~level();
  std::shared_ptr<actor> GetActor(int id);

  /*  Clean up sprites and actors from the level
  *
  *   Remove all actors in mActors with mRemove set to true,
  *   remove all AbstractSpriteHandlers in mParticleList with
  *   mRemove set to true.
  *
  *   TODO: Make this protected
  */
  
  void cleanUp();

  /*We only need to send mDimmension and the mActorList*/
  template <class Archive>
  void serialize(Archive &archive){
    archive(mDimmension, mActors);
  };
};
#endif

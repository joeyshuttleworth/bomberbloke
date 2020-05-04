#include <list>
#include <string>
#include <SDL2/SDL.h>

extern double _zoom;

class actor;

/* Actor which stores information about the level including the actors present and methods for updating and drawing the level */ 
class level{
 protected:
  /*name and description are information about this level*/
  std::string name;
  std::string description;
 public:
  /* Spawnpoints is a collection of coordinates where players can be spawned */ 
  std::vector<int> spawnPoints;

  /* Sprite holds the SDL_Texture which will be applied to the level background
     when draw is called */
  SDL_Texture *mpSprite = NULL;

  /* actorList holds each object in our level */
  std::list<actor*> actorList;

  /* dim_x and dim_y are the size of our level in the x and y axis respectively */
  double mDimmension[2];

  /*Draw our level on the window. Then draw every actor in actorList*/
  void  draw();

  level(double x, double y);
  level();
  level(const level &lvl);
  ~level();
};


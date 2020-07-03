#ifndef SCENE_HPP
#define SCENE_HPP
#include <list>
#include <string>
#include <SDL2/SDL.h>
#include <cereal/types/list.hpp>
#include <memory>

#include "AbstractSpriteHandler.hpp"


extern double _zoom;

class actor; class Camera;

/* Actor which stores information about the scene including the actors present and methods for updating and drawing the scene */
class scene{

protected:
  /*name and description are information about this scene*/
  std::string mName;
  std::string mDescription;
  std::shared_ptr<AbstractSpriteHandler> mpSpriteHandler;

  int mLastActorId=0;
public:
  /* Spawnpoints is a collection of coordinates where players can be spawned */
  std::vector<int*> mSpawnPoints;

  void addActor(std::shared_ptr<actor> a);
/* dim_x and dim_y are the size of our scene in the x and y axis respectively */
  double mDimmension[2];

  /*
   * mActors holds each object in our scene.
   * Cereal will only work if we use smart pointers
   * here but we should probably be using them anyway *?
   *
   */
  std::list<std::shared_ptr<actor>> mActors;

  /*Draw our scene on the window. Then draw every actor in mActors*/
  void draw(Camera *cam);
  void refreshSprites();

  /*
   * mParticleList lists our particles e.g explosions.
   * These won't be sent over the internet
   */

  std::list<std::shared_ptr<AbstractSpriteHandler>> mParticleList;

  /*  Return the midpoint of the scene. This is used by the Camera class. */

  std::array<double,2> getMidpoint(){
    std::array<double,2> ret = {mDimmension[0] / 2, mDimmension[1]/2};
    return ret;
  }

  scene(double x=10, double y=10);

  scene(const scene &lvl);
  ~scene();
  std::shared_ptr<actor> GetActor(int id);

  /*  Clean up sprites and actors from the scene
  *
  *   Remove all actors in mActors with mRemove set to true,
  *   remove all AbstractSpriteHandlers in mParticleList with
  *   mRemove set to true.
  *
  *   TODO: Make mParticleList protected
  */

  void cleanUp();

  /**
   * Iterates over all actors, and moves them to their positions for the next
   * tick whilst checking for, and handling collisions between actors
   */
  void movementUpdate();
  void physicsUpdate();

  /**
   * Uses the simple axis theorem to detect whether a collision has occurred between
   * two actors in the scene and returns a vector telling handleMovement how to
   * separate them.
   *
   * @param a, pointer to an actor
   * @param b, pointer to an actor
   * @return A 4d vector, the first two elements are a vector describing how a
   *         should be moved, the latter two describe the seperation vector for
   *         b.
   */
  std::array<double, 4> detectCollision(std::shared_ptr<actor> a, std::shared_ptr<actor> b);
  /*
   *   Update each actor in the scene. This could be used to implement
   *   game specific logic in the future
   */
  void updateSprites();
  virtual void update();
  virtual bool isFinished(){
    return false;
  }

  /*We only need to send mDimmension and the mActorList*/
  template <class Archive>
  void serialize(Archive &archive){
    archive(mDimmension, mActors);
  }
};

#endif
#ifndef SCENE_HPP
#define SCENE_HPP
#include <list>
#include <string>
#include <mutex>
#include <SDL2/SDL.h>
#include <cereal/types/list.hpp>
#include <memory>
#include "AbstractSpriteHandler.hpp"
#include "AbstractCollider.hpp"

extern double _zoom;

class actor; class Camera; class AbstractHudElement; class NetServer; class NetClient;

/* Class which stores information about the scene including the actors present and methods for updating and drawing the scene */
class scene{
  friend NetClient;
  friend NetServer;
protected:
  bool mNewGame = false;
  /*name and description are information about this scene*/
  std::string mName;
  std::string mDescription;
  std::shared_ptr<AbstractSpriteHandler> mpSpriteHandler;
  int mState = 0;
  int mLastActorId=0;

  /*
   * The camera object that we will use to draw the scene.
   */
  std::shared_ptr<Camera> mpCamera;

public:

  bool getNewGame(){return mNewGame;}

  virtual void handleCommand(std::string){}

  std::shared_ptr<Camera> getCamera(){
    return mpCamera;
  }

    void SetCamera(std::shared_ptr<Camera> c){
        mpCamera = c;
    }

  int getState(){return mState;}

  /**  onResize
   *
   *  Update the camera to reflect the new window size.
   */

  virtual void onResize();

  /* Spawnpoints is a collection of coordinates where players can be spawned */
  std::vector<int*> mSpawnPoints;

  void addActorWithId(std::shared_ptr<actor> a);

  void addActor(std::shared_ptr<actor> a);

  /* dim_x and dim_y are the size of our scene in the x and y axis respectively */
  double mDimmension[2];

  /*
   * mActors holds each object in the scene. For example, the player object,
   * obstacles, enemies and pickups.
   *
   */
  std::list<std::shared_ptr<actor>> mActors;

  /**
   * HUD elements drawn on top of the scene
   */
  std::list<std::shared_ptr<AbstractHudElement>> mHudElements;

  /**
   * Computes positions of HUD elements based on camera parameters.
   * Must be called when the window is created or resized.
   */
  void updateHudPositions();

  /*Draw our scene on the window. Then draw every actor in mActors*/
  virtual void draw();
  void drawActors();
  void drawHud();
  void drawParticles();
  void refreshSprites();

  /*
   * mParticleList lists our particles e.g explosions.
   * These won't be sent over the internet
   */

  std::list<std::shared_ptr<AbstractSpriteHandler>> mParticleList;

  /*  Return the midpoint of the scene. This is used by the Camera class. */

  std::array<double,2> getMidpoint(){
    std::array<double,2> ret = {{ mDimmension[0] / 2, mDimmension[1]/2 }};
    return ret;
  }

  scene(double x=10, double y=10);

  scene(const scene &lvl);

  virtual ~scene(){
      std::lock_guard<std::mutex> guard{mMutex};
  }

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
   *  A virtual function which can be overridden by child classes to implement
   *  game specific logic.
   */

  virtual void logicUpdate(){}

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

  /**
   * @param  A shared pointer to some collider type
   *
   * @return A std vector of pointers to all the actors in the level which are colliding with the given collider
   *
   */
  std::list<std::shared_ptr<actor>> ActorsCollidingWith(AbstractCollider* p_collider);

  /*We only need to send mDimmension and the mActorList*/
  template <class Archive>
  void serialize(Archive &archive){
    archive(mDimmension, mActors);
  }

  /**
   * Called by the engine whenever any input is detected.
   * Used primarily to update interactive HUD elements.
   */
  virtual void onInput(SDL_Event *event);

  /*   */

  bool linkActorToPlayer(std::shared_ptr<actor>&, unsigned int);


    std::mutex mMutex;
};

#endif

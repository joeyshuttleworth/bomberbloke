#ifndef SCENE_HPP
#define SCENE_HPP

#include <list>
#include <string>
#include <mutex>
#include <cereal/types/list.hpp>
#include <memory>

// TODO remove
#include <SDL2/SDL.h>

#include "AbstractCollider.hpp"
#include "IOSystem.hpp"
#include "threads.hpp"
#include "cereal_archives.hpp"

extern double _zoom;

class actor; class Camera; class AbstractHudElement; class NetServer; class NetClient; class AbstractSpriteHandler;

/* Class which stores information about the scene including the actors present and methods for updating and drawing the scene */
class scene{
  friend NetClient;
  friend NetServer;
protected:

  void removeAllActors();

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

  std::mutex mMutex;

  std::shared_ptr<scene> mpNextScene = nullptr;

  IOSystem& mrIOSystem;

public:

  void initGraphics();

  std::shared_ptr<scene> getNextScene(){return mpNextScene;};
  void setNextScene(std::shared_ptr<scene> s){mpNextScene = s;};

  IGraphicsManager& getGraphicsManager(){return mrIOSystem.getGraphicsManager();};
  ISoundManager& getSoundManager(){return mrIOSystem.getSoundManager();};

  std::array<double, 2> getDimension(){return mDimension;}

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
  std::array<double, 2> mDimension = {0, 0};

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

  std::list<std::shared_ptr<AbstractSpriteHandler>> mParticles;

  /*  Return the midpoint of the scene. This is used by the Camera class. */

  std::array<double,2> getMidpoint(){
    std::array<double,2> ret = {{ mDimension[0] / 2, mDimension[1]/2 }};
    return ret;
  }

  virtual void init();

  scene() : scene(_fallback_IO_system){}

  scene(IOSystem& io_system_contex, double x=10, double y=10);

  virtual ~scene(){
      LOCK_GUARD(mMutex);
  }

  std::shared_ptr<actor> GetActor(int id);

  /*  Clean up sprites and actors from the scene
  *
  *   Remove all actors in mActors with mRemove set to true,
  *   remove all AbstractSpriteHandlers in mParticles with
  *   mRemove set to true.
  *
  *   TODO: Make mParticles protected
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

  /**
   * Called by the engine whenever any input is detected.
   * Used primarily to update interactive HUD elements.
   */
  virtual void onInput(SDL_Event *event);

  /*   */

  bool linkActorToPlayer(std::shared_ptr<actor>&, int);

  /*We only need to send mDimension and the mActorList*/
  template <class Archive>
  void serialize(Archive &archive){
    archive(mDimension[0], mDimension[1]);
  }

  IOSystem& getIOSystem(){
    return mrIOSystem;
  }

};



#endif

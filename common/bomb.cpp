#include "bomb.hpp"
#include "CreationEvent.hpp"
#include "Explosion.hpp"
#include "actor.hpp"
#include "bloke.hpp"
#include "bomberbloke.h"
#include "engine.hpp"
#include "NetServer.hpp"
#include "CreationEvent.hpp"
#include "NavGrid.hpp"

void
bomb::init(bloke* bloke)
{
  mDimension[0] = BOMB_SIZE;
  mDimension[1] = BOMB_SIZE;
  if (bloke) {
    mPlacedById = bloke->mId;
    mPower = bloke->mPower;
  } else {
    log_message(ERR, "Bomb placed by malformed actor");
  }

  // Init sound

  /* Play explosion sound effect */
  int randIndex = std::rand() % N_EXPLOSION_SOUNDS;
  ISoundManager& sfx = mrIOSystem.getSoundManager();

  Sound snd(mExplosionSoundNames[randIndex]);
  mpExplosionSound = sfx.cloneSound(snd);

  return;
}

void
bomb::handleCommand(std::string command)
{
  if (mRemove == false) {
    if (command == "+kill" && _server) {
      explode();
    }
  }
  return;
}

void
bomb::update()
{
  /*Bomb collision is only turned on when the actor which placed it has
    moved away*/
  if (mPlacedById != 0) {
    if (mCollides == false) {
      std::shared_ptr<actor> placed_by = _pScene->GetActor(mPlacedById);
      /*TODO this is wrong. Use a corner or midpoint here*/
      if (placed_by) {
        if (std::abs(mPosition[0] - placed_by->mPosition[0]) >
            0.5 * (mDimension[0] + placed_by->mDimension[0])) {
          mCollides = true;
        } else if (std::abs(mPosition[1] - placed_by->mPosition[1]) >
                   0.5 * (mDimension[1] + placed_by->mDimension[1])) {
          mCollides = true;
        }
      }
    }
  }
  if (mTimer == 0)
    explode();
  else
    mTimer--;
  return;
}

std::vector<BombPath>
bomb::identifyTargetSquares()
{
  /** Create stacks of squares that can be iterated in a FIFO fashion **/
  std::vector<BombPath> targets;
  int i = int(mPosition[0]); int j = int(mPosition[1]);

  // Square bomb is sat on
  targets.push_back({
    { std::make_pair(i, j)}
  });

  // Horizontal
  BombPath left;
  BombPath right;
  for(int power = 1; power <= mPower; power++) {
    left.squares.emplace_back(i-power,j);
    right.squares.emplace_back(i+power,j);
  }

  // Vertical
  BombPath up;
  BombPath down;
  for(int power = 1; power <= mPower; power++) {
    up.squares.emplace_back(i, j+power);
    down.squares.emplace_back(i, j-power);
  }

  targets.push_back(left);
  targets.push_back(right);
  targets.push_back(up);
  targets.push_back(down);

  return targets;
}


void
bomb::explode()
{
  remove();

  if(mpScene){
    if(mpScene->getCamera()){
      mpScene->getCamera()->rumble();
    }
  }

  if (getType() == ACTOR_BIG_BOMB)
    mPower = 100;

  /* TODO include build macro here */
  if (_server) {

    std::unique_ptr<AbstractEvent> c_event = std::make_unique<CreationEvent>(mpExplosionSound);
    _net_server->broadcastEvent(std::move(c_event));

    mrIOSystem.getSoundManager().playSound(mpExplosionSound);

    std::vector<std::shared_ptr<AbstractSpriteHandler>> explosionEffects;

    /*Iterate over all the squares the bomb can reach and kill the ones if they
     * are in the right (wrong) zone.*/
    std::vector<BombPath> targets = identifyTargetSquares();

    for(const auto& path : targets) {
      for(const auto& coord : path.squares) {
        bool stopped = false; // Do not continue along blast path past this one
        bool blocked = false; // Do not explode on current square

        // Make square slightly smaller
        auto square = std::make_shared<actor>(mpScene, coord.first + _bomb_delta,
                                              coord.second + _bomb_delta,
                                              1.0 - _bomb_delta, 1.0 - _bomb_delta,
                                              false);

        std::list<std::shared_ptr<actor>> actor_list =
          _pScene->ActorsCollidingWith(square.get());

        IGraphicsManager& gfx_manager = mrIOSystem.getGraphicsManager();
        for (std::shared_ptr<actor> pActor : actor_list) {
          if (pActor.get() == this)
            continue;
          else {
            pActor->handleCommand("+kill");
            switch (pActor->getType()) {
              case ACTOR_WOODEN_CRATE:
              case ACTOR_BLOKE:
              case ACTOR_BOMB:
                stopped = true;
                break;
              case ACTOR_STONE_BLOCK:
                blocked = true;
                break;
              default:
                break;
            }
          }
        }

        if(blocked)
          break;

        explosionEffects.push_back(
                                   std::make_shared<Explosion>(gfx_manager,
                                                               coord.first, coord.second, 1, 1, false, 30, 64, 0)
                                   );
        if(stopped)
          break;
      }
    }

   /*Cast to a bloke pointer.*/
    std::shared_ptr<bloke> placed_by =
      std::dynamic_pointer_cast<bloke>(_pScene->GetActor(mPlacedById));
    if (placed_by && _server) {
      placed_by->mBombs--;
      if (getType() == ACTOR_BIG_BOMB) {
        placed_by->mBigBombPlaced = false;
      }
    }

    /* Create explosion particles on all clients */
    for(auto expl : explosionEffects) {
      std::unique_ptr<AbstractEvent> c_event(new CreationEvent(expl));
      _net_server->broadcastEvent(std::move(c_event));
    }
  }

  return;
}

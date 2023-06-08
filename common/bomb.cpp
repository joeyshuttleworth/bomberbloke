#include "bomb.hpp"
#include "CreationEvent.hpp"
#include "Explosion.hpp"
#include "actor.hpp"
#include "bloke.hpp"
#include "bomberbloke.h"
#include "engine.hpp"

void
bomb::init(bloke* bloke)
{
  mDimmension[0] = BOMB_SIZE;
  mDimmension[1] = BOMB_SIZE;
  if (bloke) {
    mPlacedById = bloke->mId;
    mPower = bloke->mPower;
  } else {
    log_message(ERR, "Bomb placed by malformed actor");
  }
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
            0.5 * (mDimmension[0] + placed_by->mDimmension[0])) {
          mCollides = true;
        } else if (std::abs(mPosition[1] - placed_by->mPosition[1]) >
                   0.5 * (mDimmension[1] + placed_by->mDimmension[1])) {
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

std::vector<std::vector<TargetSquare>>
bomb::identifyTargetSquares()
{
  /** Create stacks of squares that can be iterated in a FIFO fashion **/
  std::vector<std::vector<TargetSquare>> targets;
  int i = int(mPosition[0]); int j = int(mPosition[1]);

  auto makeSquare = [](int a, int b){
    return std::make_shared<actor>(a, b, 1, 1, false);
  };

  // Square bomb is sat on
  targets.push_back({makeSquare(i, j)});

  // Horizontal
  std::vector<TargetSquare> left;
  std::vector<TargetSquare> right;
  for(int power = 1; power <= mPower; power++) {
    left.push_back(makeSquare(i - power, j));
    right.push_back(makeSquare(i + power, j));
  }

  // Vertical
  std::vector<TargetSquare> up;
  std::vector<TargetSquare> down;
  for(int power = 1; power <= mPower; power++) {
    up.push_back(makeSquare(i, j + power));
    down.push_back(makeSquare(i, j - power));
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

  if (getType() == ACTOR_BIG_BOMB)
    mPower = 100;

  if (_server) {
    std::vector<std::shared_ptr<AbstractSpriteHandler>> explosionEffects;

    /*Iterate over all the squares the bomb can reach and kill the ones if they
     * are in the right (wrong) zone.*/
    std::vector<std::vector<TargetSquare>> targets = identifyTargetSquares();
    for(const auto& stack : targets) {
      for(const auto& square : stack) {
        explosionEffects.push_back(
          std::make_shared<Explosion>(int(square->mPosition[0]),
                                      int(square->mPosition[1]),
                                      1, 1, false
                                      )
        );

        std::list<std::shared_ptr<actor>> actor_list =
          _pScene->ActorsCollidingWith(square.get());
        bool stopped = false;
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
              default:
                break;
            }
          }
        }
        if (stopped)
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
      _net_server.broadcastEvent(c_event);
    }
  }

  return;
}

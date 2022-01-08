#include "bloke.hpp"
#include "BigBomb.hpp"
#include "CommandEvent.hpp"
#include "bomb.hpp"
#include "bomberbloke.h"
#include <cereal/archives/json.hpp>
#include <sstream>
#include <string>

const std::string PLACE_BOMB_SOUND_NAME = "place_bomb";

bloke::bloke(double x, double y, bool collides, uint64_t colour)
  : actor(x, y, DEFAULT_BLOKE_SIZE, DEFAULT_BLOKE_SIZE, true)
{
  mCollides = collides;
  mColour = colour;
  mPosition[0] = x;
  mPosition[1] = y;
}

void
bloke ::accelerate()
{
  // Maximum speed in units per tick
  double max_speed;

  max_speed = (BASE_SPEED + DEFAULT_SPEED_INCREASE * mMaxSpeed) / TICK_RATE;

  /*Count how many directions we are accelerating in*/
  double acceleration_magnitude = pow(mAcceleration[1] * mAcceleration[1] +
                                        mAcceleration[0] * mAcceleration[0],
                                      0.5);

  const double velocity_increase[2] = {
    max_speed * ACCELERATION_RATIO * mAcceleration[0] / acceleration_magnitude,
    mMaxSpeed * ACCELERATION_RATIO * mAcceleration[1] / acceleration_magnitude
  };

  for (int i = 0; i < 2; i++) {
    if (mAcceleration[i] != 0) {
      mVelocity[i] = mVelocity[i] + velocity_increase[i];
      if (std::abs(mVelocity[i]) > max_speed)
        mVelocity[i] = mVelocity[i] * max_speed / std::abs(mVelocity[i]);
    } else {
      double decceleration =
        (mVelocity[i] > 0) ? -max_speed * 0.5 : max_speed * 0.5;
      if (std::abs(decceleration) > std::abs(mVelocity[i]))
        mVelocity[i] = 0;
      else
        mVelocity[i] += decceleration;
    }
  }
  /*Finally, if we are a client, construct a command event
    to send this move command to the server*/
  return;
}

void
bloke ::handleCommand(std::string command)
{
  if (_server) {
    /*True if the key is pressed down- false if it is up*/
    bool key_down = (command[0] == '+');
    if (command == "kill" || command == "+kill") {
      remove();
      return;
    }

    /*Handle movement commands*/
    const std::string direction_strings[4] = { "up", "right", "down", "left" };

    for (unsigned int i = 0; i < 4; i++) {
      if (command.substr(1) == direction_strings[i]) {
        if (key_down) {
          if (mDirectionsHeld[i] == false) {
            mDirectionsHeld[i] = true;
            mAcceleration[(i + 1) % 2] = (i < 2) ? 1 : -1;
          }
        } else {
          if (mDirectionsHeld[i] == true) {
            mDirectionsHeld[i] = false;
            if (mDirectionsHeld[(i + 2) % 4])
              mAcceleration[(i + 1) % 2] = (i < 2) ? -1 : 1;
            else {
              mAcceleration[(i + 1) % 2] = 0;
            }
          }
        }
      }
    }

    if (command == "+bomb") {
      place_bomb();
    }

    /*Command(s) that take parameters go here*/
    std::list<std::string> tokens = split_to_tokens(command);

    if (tokens.back() == "accel" && tokens.size() == 3) {
      auto counter = tokens.begin();
      double x_accel = std::stod(*counter);
      counter++;
      double y_accel = std::stod(*counter);
      if (std::abs(x_accel) <= 1 && std::abs(y_accel) <= 1) {
        mAcceleration[0] = x_accel;
        mAcceleration[0] = y_accel;
      }
    }
  } else {
    if (command == "+bomb") {
      soundManager.playSound(mPlaceBombSound);
    }
  }
  return;
}

void
bloke ::update()
{
  accelerate();
  return;
}

void bloke ::init(){
  auto sprite = std::make_shared<PlaceHolderSprite>(
                                                    mPosition[0], mPosition[1], mDimmension[0], mDimmension[1]);

  auto p_player = getPlayer();

  if(p_player != nullptr)
    mColour = p_player->getColour();

  // Set colour
  sprite->setColour(mColour);

  mpSpriteHandler = sprite;

  std::stringstream sstream;
  sstream << "colour of bloke is " << std::hex << mColour;
  log_message(DEBUG, sstream.str());

  mProperties = std::make_shared<GamePlayerProperties>();
  mPlaceBombSound = soundManager.createSound(PLACE_BOMB_SOUND_NAME);
  mPlaceBombSound->mGroup = SOUND_FX;
}

void
bloke ::place_bomb()
{
  if (mBombs < mMaxBombs + 1) {
    std::shared_ptr<bomb> new_bomb = std::make_shared<bomb>(this);
    if (mBigBomb && !mBigBombPlaced) {
      new_bomb = std::make_shared<BigBomb>(this);
      mBigBombPlaced = true;
    }
    new_bomb->init(this);
    _pScene->addActor(new_bomb);
    mBombs++;
  }
  return;
}

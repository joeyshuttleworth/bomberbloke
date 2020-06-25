#ifndef BOMBERBLOKE_HPP
#define BOMBERBLOKE_HPP

#include "engine.hpp"

#define DEFAULT_BLOKE_SIZE 1
#define ACCELERATION_RATIO 0.5
#define DECCELERATION_RATIO 0.1
#define DEFAULT_MAX_SPEED double(0.5 / TICK_RATE)
#define DEFAULT_BOMB_TIMER 10*TICK_RATE
#define BOMB_SIZE 0.25

extern double _zoom;
extern double _bloke_size[2];

extern unsigned int  _default_bomb_timer;
class bomb;
class bloke;

enum powerups{
  SATELLITE,
  KICK,
  PENETRATION
};

enum actor_types{
  ACTOR_WOODEN_CRATE,
  ACTOR_METAL_CRATE,
  ACTOR_EXPLOSIVE_CRATE,
  ACTOR_DEATH_BLOCK,
  ACTOR_BLOKE,
  ACTOR_BOMB
};

enum flags{
  FLAG_NONE = 0,
  FLAG_DEAD,
};

enum block_types{
  EMPTY,
  RESERVED,
  SPAWN_POINT,
  WOOD
};

#endif

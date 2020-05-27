#include "../engine/engine.h"

#define DEFAULT_BLOKE_SIZE 1
#define ACCELERATION_RATIO 0.5
#define DECCELERATION_RATIO 0.1
#define DEFAULT_MAX_SPEED double(0.5 / TICK_RATE)
#define DEFAULT_BOMB_TIMER 5*TICK_RATE
#define BOMB_SIZE 1

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
  WOOD_BLOCK,
  METAL_BLOCK,
  EXPLOSIVE_BLOCK,
  DEATH_BLOCK,
  BLOKE,
  BOMB
};

enum flags{
  FLAG_NONE = 0,
  FLAG_DEAD,
};

#include "bloke.h"
#include "bomb.h"

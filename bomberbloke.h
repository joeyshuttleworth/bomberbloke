#include "engine/engine.h"
#include "engine/level.h"
#include "engine/controls.h"
#include "engine/actor.h"
#include "engine/player.h"
extern "C"{
#include "engine/net.h"
}
#include <SDL2/SDL.h>

#define DEFAULT_BLOKE_SIZE 1
#define ACCELERATION_RATIO 1
#define DECCELERATION_RATIO 0.1
#define DEFAULT_MAX_SPEED 5
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

enum direction{
  DIR_NONE,
  DIR_UP,
  DIR_RIGHT,
  DIR_LEFT,
  DIR_DOWN
};

enum block_types{
  EMPTY,
  WOOD,
  METAL,
  EXPLOSIVE,
  DEATH,
};

enum flags{
  FLAG_NONE = 0,
  FLAG_DEAD,
};

#include "bloke.h"
#include "bomb.h"

#include "engine/engine.h"
#include "engine/level.h"
#include "engine/controls.h"
#include "engine/actor.h"
#include "engine/player.h"
#include <SDL2/SDL.h>

#define DEFAULT_BLOKE_SIZE 1
#define ACCELERATION_RATIO 1
#define DECCELERATION_RATIO 0.1
#define DEFAULT_MAX_SPEED 5
#define DEFAULT_BOMB_TIMER 5*TICK_RATE
extern double       _zoom;
extern double       _bloke_size[2];
class bomb;
class bloke;

enum powerups{
  SATELLITE,
  KICK,
  PENETRATION
};

enum direction{
  NONE,
  UP,
  RIGHT,
  LEFT,
  DOWN
};


enum block_types{
  EMPTY,
  WOOD,
  METAL,
  EXPLOSIVE,
  DEATH,
};

class bomb : public actor {
  double timer;
  Uint8 power;
  bloke *placed_by;
  bool penetration;
  bool big_bomb;
  bool satellite;
 public:
  void init(bloke*, int, int);
  bomb();
  void explode();
  void update();
};

class bloke : public actor{
 private:
  std::list<int> powerups;
  bool accelerated = false;
  void place_bomb();
 public:
  void  draw();
  void  kick(bomb *bomb, uint8_t direction);
  void  kill();
  SDL_Surface *surf;
  int init();
  using actor::actor;
  void handle_command(std::string command);
  void accelerate(int);
  double max_speed = double(DEFAULT_MAX_SPEED) / double(TICK_RATE);
  Uint8 max_bombs;
  Uint8 bombs;
  Uint8 power;
  void update();
};


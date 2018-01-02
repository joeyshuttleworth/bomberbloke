#include <cmath>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <iostream>
#include <algorithm>
#include <list>
#include <vector>
#include <iterator>
#include <fstream>
#include <array>
#include <algorithm>

#define TICK_RATE     64
#define NET_RATE      64
#define OUT_OF_BOUNDS 127
#define DEFAULT_ZOOM  50
#define DEFAULT_ACTOR_SIZE 1
#define DEFAULT_WINDOW_WIDTH  700
#define DEFAULT_WINDOW_HEIGHT 700
class level;
class actor;
class local_p;

void client_loop(level *current_level);
void log_message(int, char*);
void handle_system_command(std::list<std::string>);
void init_engine(level*);
void *console_loop(void *);
std::list<std::string> split_to_tokens(std::string);

extern SDL_Window  *_window;
extern SDL_Surface *_surface;
extern double _screen_offset[2];
extern bool _draw;
extern bool _server;
extern double _zoom;
extern unsigned int _state;

typedef struct{
  SDL_Scancode scancode;
  std::string command;
} command_binding;

enum LOG_LEVELS{
  DEBUG,
  INFO,
  WARNING,
  ERROR,
  CRITICAL
};

enum player_types{
  LOCAL,
  NETWORK
};

enum states{
  PAUSED,
  STOPPED,
  EXIT,
  PLAYING
};

extern const std::vector<command_binding> _default_bindings;
extern std::list<local_p> _local_player_list;
const std::array<std::string, 2> _system_commands  = {"bind", "set"};

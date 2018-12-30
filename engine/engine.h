#include <cmath>
#include <SDL2/SDL.h>
#include <iostream>
#include <algorithm>
#include <list>
#include <vector>
#include <iterator>
#include <fstream>
#include <array>
#include <algorithm>
#include <ctime>
extern "C"{
#include "net.h"
}

#define OUT_OF_BOUNDS 127
#define DEFAULT_ZOOM  50
#define DEFAULT_ACTOR_SIZE 1
#define DEFAULT_WINDOW_WIDTH  700
#define DEFAULT_WINDOW_HEIGHT 700
#define MIN_VELOCITY 0.00000000001

class actor;
class level;
class local_p;
class network_p;

struct net_float;

void send_to_all(net_message*);
void send_to_list(net_message*, std::list<network_p>);
void new_game(std::string);
void engine_new_game(std::string);
void engine_start_game();
void client_loop();
void server_loop();
void log_message(int, const char*);
void handle_system_command(std::list<std::string>);
void handle_input(level*);
void handle_movement();
void init_engine();
void *console_loop(void *);
void draw_screen();
void send_to_all(net_message*);
std::list<std::string> split_to_tokens(std::string);

extern SDL_Window  *_window;
extern SDL_Surface *_surface;
extern SDL_Renderer *_renderer;
extern double _screen_offset[2];
extern bool _draw;
extern bool _server;
extern bool _halt;
extern double _zoom;
extern unsigned int _state;
extern std::list<network_p> _client_list;
extern pthread_t net_receive, read_console;

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

typedef struct{
  uint16_t id;
  net_float position[2];
  net_float velocity[2];
} move;

extern const std::vector<command_binding> _default_bindings;
extern std::list<local_p> _local_player_list;
const std::array<std::string, 2> _system_commands  = {"bind", "set"};

#include "state.h"
#include "level.h"
#include "actor.h"
#include "player.h"

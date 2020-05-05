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
#include <signal.h>
#include <SDL_joystick.h>
#include <thread>

#define DEFAULT_ZOOM  50
#define DEFAULT_ACTOR_SIZE 1
#define DEFAULT_WINDOW_WIDTH  700
#define DEFAULT_WINDOW_HEIGHT 700
#define MIN_VELOCITY 0.00000000001
#define TICK_RATE 64

class actor;
class level;
class localPlayer;
class networkPlayer;

void exit_engine(int);
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
void console_loop();
void draw_screen();
SDL_Joystick* handle_input_controller();
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
extern std::list<networkPlayer> _client_list;
extern std::string _nickname;
extern SDL_Joystick* _controller;
extern bool _controller_connected;


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

class level;

extern level _level;
extern unsigned int _tick;
extern const std::vector<command_binding> _default_bindings;
extern std::list<localPlayer> _local_player_list;
const std::array<std::string, 2> _system_commands  = {"bind", "set"};

#include "state.h"
#include "level/level.h"
#include "actor/actor.h"
#include "player/player.h"
#include "player/localPlayer.h"
#include "player/networkPlayer.h"

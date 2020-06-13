#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <map>
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
#define MIN_VELOCITY 1e-8
#define TICK_RATE 64

class actor;
class level;
class LocalPlayer;
class NetworkPlayer;
class AbstractPlayer;

void exit_engine(int);
void new_game(std::string);
void engine_new_game(std::string);
void engine_start_game();
void client_loop();
void server_loop();
void log_message(int, std::string);
bool handle_system_command(std::list<std::string>);
void handle_input();
void handle_movement();
void init_engine();
void console_loop();
void draw_screen();
SDL_Joystick* handle_input_controller();

/* Removes white space and turns a string into a list of words.
   This is used for parsing commands.
*/
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
extern std::list<std::shared_ptr<AbstractPlayer>> _player_list;
extern std::string _nickname;
extern SDL_Joystick* _controller;
extern bool _controller_connected;


typedef struct{
  SDL_Scancode scancode;
  std::string command;
} CommandBinding;

enum LOG_LEVEL{
  DEBUG = 0,
  INFO,
  WARNING,
  ERROR,
  CRITICAL,
  ALL
};

const std::string LOG_LEVEL_STRINGS[] = {
  "Debug",
    "Info",
    "Warning",
    "ERROR",
    "ALL"
};

enum player_types{
  LOCAL,
  NETWORK
};

class level;

/*Define global variables.
  TODO: consider changing these so that they're all contained in one class
 */
class  ServerInfo;
extern ServerInfo _server_info;
extern level _level;

extern unsigned int _tick;
extern std::vector<CommandBinding> _default_bindings;
extern std::list<LocalPlayer> _local_player_list;
const std::array<std::string, 2> _system_commands  = {{"bind", "set"}};


/*  A function defined by the game / test called each tick */
void gameUpdate();


#include "AbstractSpriteHandler.hpp"
#include "state.h"
#include "NetworkPlayer.hpp"
#include "LocalPlayer.hpp"
#include "level.hpp"
#include "actor.hpp"
#include "config.hpp"

#endif

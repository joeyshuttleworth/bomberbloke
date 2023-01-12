#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <map>
#include <cmath>
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <SDL3/SDL_joystick.h>
#include <SDL3/SDL_main.h>
#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>
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
#include <thread>
#include <limits>
#include <mutex>

#define DEFAULT_ZOOM  50
#define DEFAULT_ACTOR_SIZE 1
#define DEFAULT_WINDOW_WIDTH  700
#define DEFAULT_WINDOW_HEIGHT 700
#define MIN_VELOCITY 1e-8
#define TICK_RATE 64
#define DOUBLE_UNSET std::numeric_limits<double>::quiet_NaN()

// Pretty arbitrary constant to set a reasonable limit for id searches doesn't
// really matter as long as it's greater than the mamximum number of players and
// actors.
#define BLOKE_MAX_ID 10000


/* ENUM to track what the client/server is doing */

enum state{
           PAUSED,
           STOPPED,
           EXIT,
           JOINING,
           PLAYING,
           DISCONNECTED
};

extern std::mutex _scene_mutex;

class actor;
class scene;
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
void set_draw(bool);
SDL_Joystick* handle_input_controller();
void add_player(std::shared_ptr<AbstractPlayer>);


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
extern std::list<std::shared_ptr<AbstractPlayer>> _player_list;
extern std::string _nickname;
extern SDL_Joystick* _controller;
extern bool _controller_connected;

/* Global sound manager for storing and tracking playing sounds */
#include "SoundManager.hpp"
extern SoundManager soundManager;

#include "TextManager.hpp"
extern TextManager textManager;


typedef struct{
  SDL_Scancode scancode;
  std::string command;
} CommandBinding;

enum LOG_LEVEL{
  DEBUG = 0,
  INFO,
  WARNING,
  ERR,
  CRITICAL,
  ALL
};

const std::string LOG_LEVEL_STRINGS[] =
  {"debug",
   "info",
   "warning",
   "error",
   "critical",
   "all"
  };

enum player_types{
  LOCAL,
  NETWORK
};

class scene;

/*Define global variables.
  TODO: consider changing these so that they're all contained in one class
 */
class  ServerInfo;
extern ServerInfo _server_info;
extern std::shared_ptr<scene> _pScene;
extern std::shared_ptr<scene> _pNewScene;

extern unsigned int _tick;
extern std::vector<CommandBinding> _default_bindings;
extern std::list<LocalPlayer> _local_player_list;

const std::array<std::string, 10> _system_commands  =
  {{"bind",
    "zoom",
    "draw",
    "log_level",
    "draw",
    "colour",
    "generate_config",
    "info",
    "quit",
    "resize"}};

/*  A function defined by the game / test called each tick */
void gameUpdate();

class NetClient;
class NetServer;

#include "NetClient.hpp"
#include "NetServer.hpp"
extern NetClient _net_client;
extern NetServer _net_server;

/* Define the path seperator based on the operating system*/
const std::string PATHSEPARATOR =
#ifdef _WIN32
"\\";
#else
"/";
#endif

#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>

#include "AbstractSpriteHandler.hpp"
#include "NetworkPlayer.hpp"
#include "LocalPlayer.hpp"
#include "scene.hpp"
#include "actor.hpp"
#include "config.hpp"
#include "Camera.hpp"
#endif

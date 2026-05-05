#include "engine.hpp"
#include "AbstractSpriteHandler.hpp"
#include "CommandEvent.hpp"
#include "FollowCamera.hpp"
#include "MainMenuScene.hpp"
#include "MoveEvent.hpp"
#include "NetClient.hpp"
#include "NetServer.hpp"
#include "QueryEvent.hpp"
#include "ServerInfo.hpp"
#include "ShowAllCamera.hpp"
#include "LocalPlayer.hpp"
#include "SyncEvent.hpp"
#include "assets.hpp"
#include "scene.hpp"
#include "config.hpp"
#include <cereal/archives/json.hpp>
#include <dirent.h>
#include <exception>
#include <fstream>
#include <utility>
#include <chrono>
#include <SDL2/SDL_image.h>

#include "IOSystem.hpp"


CommandQueue _command_queue;

/*  TODO: reduce number of globals */
int _log_message_level = 0;

/* Global variables tracking state */
bool _bind_next_key = false;

bool _halt = false;
unsigned int _tick = 0;

std::vector<LocalPlayer> _local_player_list;

/* Pointers to current scene and next scene for switching */
std::shared_ptr<scene> _pScene;
std::shared_ptr<scene> _pNewScene;

// Shouldn't this be inside _pScene?
// std::list<std::shared_ptr<AbstractSpriteHandler>> _particle_list;

/* TODO move to input interface */
// SDL_Joystick* _controller = nullptr;
bool _controller_connected = false;

/* TODO Move to client code - this is only relevant to client */
std::string _nickname = "bloke";

/* Move to server.cpp */
ServerInfo _server_info;

/* TODO more logging options */
std::ofstream _console_log_file;

/* Move to input interface */
std::vector<CommandBinding> _default_bindings;

/* No need to be global. Similar variable already exists in NetServer and NetClient.*/
std::list<std::shared_ptr<AbstractPlayer>> _player_list;

/* Avoid different threads modifying the scene at the same time */
DECLARE_MUTEX(_scene_mutex);

/* Shouldn't be globals here. Move to client.cpp and server.cpp */
std::unique_ptr<NetClient> _net_client = std::make_unique<NetClient>();
std::unique_ptr<NetServer> _net_server = std::make_unique<NetServer>();

IOSystem _fallback_IO_system;

void
exit_engine(int signum)
{


  _halt = true;

  std::cout << "\nNow exiting the BLOKE engine. Hope you had fun. Wherever you "
               "are, we at the BLOKE project hope we have made your day just a "
               "little bit brighter. See you next time around! :)\n";
  signal(SIGINT, NULL);
#ifndef _WIN32
  std::cout << "Received signal " << strsignal(signum) << ".\nExiting...\n";
#endif

  _pScene = nullptr;
  std::this_thread::sleep_for(std::chrono::seconds(1));
  SDL_Quit();
  return;
}


void
init_engine
(IOSystem&, bool server)
{
  if(server)
    _net_server = std::unique_ptr<NetServer>(new NetServer());
  else
    _net_client = std::unique_ptr<NetClient>(new NetClient());

  signal(SIGINT, exit_engine);
  SDL_Init(SDL_INIT_EVERYTHING);

  int flags = IMG_Init(IMG_INIT_PNG);
  if(!(flags & IMG_INIT_PNG))
    log_message(ERR, "PNG init failed: " + std::string(IMG_GetError()));

  /*  Open a log file  */
  _console_log_file.open("/tmp/bloke.log");

  /* Initialise the controller if it exists */
  // _controller = handle_input_controller();
  // _controller_connected = _controller != nullptr ? true : false;


  LAUNCH_THREAD_DETACH(console_loop);

  return;
}


/* Handle input i.e. controls and window events here */
void
handle_input(IOSystem& ctx)
{
  IInputManager& input_manager = ctx.getInputManager();
  input_manager.handleInput(*_pScene);
}


// SDL_Joystick*
// handle_input_controller()
// {
//   SDL_Init(SDL_INIT_JOYSTICK);
//   if (SDL_NumJoysticks() > 0) {
//     std::cout << "Controlled connected\n ";
//     return SDL_JoystickOpen(0); // return joystick identifier
//   } else
//     return NULL; // no joystick found
// }

void
logic_loop()
{
  return;
}

/* TODO move elsewhere */
void
log_message(int scene, std::string str)
{
  if (scene > ALL)
    scene = ALL;

  /* Output to our log file */
  _console_log_file << str << "\n";

  if (scene < _log_message_level) {
    /*Ignore the message*/
    return;
  } else {
    std::cout << _tick << "\t " << LOG_LEVEL_STRINGS[scene] << ": " << str
              << std::endl;
    return;
  }
}

void
load_config(std::string fname)
{
  std::ifstream fin(fname, std::ifstream::in);
  char buf[32];
  bool in_player = false;
  if (!fin.good())
    return;
  while (fin.getline(buf, 32)) {
    std::string bind_string = std::string(buf);
    std::transform(
      bind_string.begin(), bind_string.end(), bind_string.begin(), ::tolower);
    if (in_player) {
      if (bind_string.find("end player")) {
        in_player = false;
      }
    } else {
      if (bind_string.find("start player")) {
        in_player = true;
      }
    }
  }
}

bool
handle_system_command(IOSystem& ctx, Tokens tokens)
{
  if (tokens.size() == 0)
    return true;

  std::string command = tokens.front();

  bool key_down = !(command[0] == '-');

  command = (command[0] == '+') ? command.substr(1) : command;
  command = command[0] == '-' ? command.substr(1) : command;

  if (_server && command == "kick") {
    if (tokens.size() != 3) {
      log_message(
        ERR,
        "kick command requires 3 arguments kick <playername> <\"reason\">");
      return false;
    }
    auto iter = tokens.begin();
    iter++;
    _net_server->disconnectPlayer(*iter, tokens.back());
    return true;
  }

  else if (command == "pause" && !key_down){
    _pScene->handleCommand("pause");
  }

  else if (command == "toggle_pause"){
    if(!key_down)
      _pScene->handleCommand("toggle_pause");
  }

  else if (command == "players" && _server) {
    _net_server->printPlayers();
  }

  else if (command == "new" && _server) {
    log_message(INFO, "starting new game");
    new_game(ctx, "");
    _net_server->syncPlayers();
  }

  else if (command == "zoom" && !_server && !key_down) {
    if (tokens.size() == 2) {
      std::string arg = tokens.back();

      auto camera = _pScene->getCamera();
      double zoom = DOUBLE_UNSET;

      if (camera)
        zoom = camera->getZoom();

      double val = DOUBLE_UNSET;

      if (arg == "all") {
        _pScene->handleCommand(arg);
      }

      else if (arg == "follow") {
        _pScene->handleCommand(arg);
      }

      else {
        if (arg[0] == '*') {
          try {
            val = std::stod(arg.substr(1));
          } catch (std::exception& exc) {
          }
          std::cout << zoom << " " << val << "\n";
          zoom = zoom * val;
        }

        else {
          try {
            val = std::stod(arg);
          } catch (std::exception& exc) {
          }
          zoom = val;
        }
        if (camera) {
          log_message(DEBUG, "setting zoom to " + std::to_string(zoom));
          camera->setZoom(zoom);
        } else {
          // TODO
        }
      }
    } else {
      log_message(ERR, "zoom requires exactly one argument");
    }
  }

  else if (command == "nickname" && !_server) {
    if (tokens.size() != 2)
      log_message(ERR, "nickname requires exactly one argument");
    else {
      _nickname = tokens.back();
      log_message(INFO, "nickname is now " + _nickname);
    }
  }

  else if (command == "disconnect") {
    /*TODO:*/ if (_server) {

    } else {
      _net_client->disconnectClient();
      _pScene = std::make_unique<MainMenuScene>(_pScene->getIOSystem(), 15, 15);
    }
  }

  else if (command == "draw") {
    IGraphicsManager& gfx = ctx.getGraphicsManager();
    if (tokens.size() == 2) {
      if (tokens.back() == "on") {
        gfx.setDraw(true);
      } else if (tokens.back() == "off") {
        gfx.setDraw(false);
      } else {
        log_message(ERR,
                    "Couldn't parse command - " + command + tokens.back() +
                      " the options for 'draw' are 'on' and 'off'");
        return false;
      }
    }

    else {
      log_message(ERR, "draw command requires one argument");
      return false;
    }
  }

  // else (command == "msg"){
  //     std::string msg_string = std::accumulate(vec.begin(),
  //                                              vec.end(),
  //                                              std::string(" "));
  //     std::shared_ptr<AbstractEvent> testEvent = std::make_shared<MessageEvent>("hello w0rld?");
  //   }

  else if (!_server && command == "open") {
    if (tokens.size() == 2) {
        /**
           NetClient::connectClient returns true or false. Return
           this value
        */
        std::string address = tokens.back();
        if (!_net_client->joinBlokeServer(address, _nickname)) {
          log_message(INFO, "failed to connect to server");
          return false;
        }
      } else {
        /* TODO allow port number as a separate argument? */
        log_message(ERR, "Open requires exactly one argument");
      }
    }

    else if (command == "info") {
      QueryEvent e(_nickname);
    cereal::JSONOutputArchive oArchive(std::cout);
    oArchive(e);
  }

  else if (command == "log_level") {
    if (tokens.size() != 2) {
      log_message(ERR, "Command: loglevel requires exactly one argument.");
      return false;
    } else {
      // Critical is our highest warning level
      auto iterator = tokens.begin();
      iterator++;
      std::string input_string = *iterator;
      for (unsigned int i = 0; i <= CRITICAL; i++) {
        if (input_string == LOG_LEVEL_STRINGS[i]) {
          _log_message_level = i;
          log_message(ALL, "Log level set to " + LOG_LEVEL_STRINGS[i]);
        }
      }
    }
  }

  else if (command == "quit") {
    exit_engine(0);
  }

  else if (command == "generate_config") {
    if (tokens.size() == 1) {
      GenerateConfig("generated_config.conf");
    } else if (tokens.size() == 2) {
      std::string fname = tokens.back();
      GenerateConfig(fname);
    } else {
      log_message(ERR, "To many arguments supplied to generate_config");
    }
  }

  else if (command == "resize") {
    if (tokens.size() == 3) {
      auto i = tokens.begin();
      i++;
      /*TODO: Try catch here*/
      std::cout << *i << "\n";
      int x = std::stoi(*i);
      i++;
      int y = std::stoi(*i);

      IGraphicsManager& gfx = ctx.getGraphicsManager();
      gfx.resizeWindow(x, y);
    } else {
      log_message(ERR, "Incorrect number of arguments for resize");
    }
  }

  else if (command == "bind") {
    IInputManager& input_manager = ctx.getInputManager();
    if (tokens.size() == 3) {
      auto i = tokens.begin();
      i++;
      CommandBinding new_command;
      new_command.command = *i;
      i++;

      unsigned int key_number = 0;
      try
        {
          key_number = std::stoi(*i);
        }
      catch (std::invalid_argument &e) {
        std::cout << e.what();
        return false;
      }

      new_command.scancode = input_manager.getKeyScanCode(key_number);

      _local_player_list.front().mControlScheme.push_back(new_command);
      log_message(INFO,
                  "Successfully bound command " + new_command.command + " to " +
                    std::to_string(new_command.scancode));
    } else if (tokens.size() == 2) {
      _bind_next_key = true;
      auto i = tokens.begin();
      i++;

      input_manager.setNextBindCommand(*i);

      log_message(INFO,
                  "binding next keypress to command: " + *i);
    }
  }   // Colour command: request to change the players colour
  else if (!_server && command == "colour"){
    // Player has requested to change colour
    if(tokens.size()!=2)
      log_message(ERR, "colour command requires exactly one argument");
    else{
      // Send request
      std::string command_to_send = command + " " + tokens.back();
      std::unique_ptr<AbstractEvent> c_event(new CommandEvent(command_to_send));
      _net_client->mConnector->broadcastEvent(std::move(c_event));
      log_message(INFO, "Requesting to change player colour to " + tokens.back());
      log_message(DEBUG, "Sending command \"" + command_to_send + "\"");
    }
  }
  else {
    log_message(ERR, "unknown command: " + command);
  }
  return true;
}

Tokens
split_to_tokens(std::string str)
{
  /*  First remove all unnecessary whitespace */
  std::string clean_str;
  clean_str.reserve(str.length());

  bool space = true;
  bool in_quotes = false;
  for (char ch : str) {
    if (ch == '\"') {
      in_quotes = !in_quotes;
    }
    if (in_quotes) {
      clean_str.push_back(ch);
      continue;
    } else if (!in_quotes && !space && std::isspace(ch)) {
      space = true;
      clean_str.push_back(ch);
    } else if (space && !std::isspace(ch)) {
      space = false;
      clean_str.push_back(ch);
    } else if (!space && !std::isspace(ch))
      clean_str.push_back(ch);
  }
  if (clean_str.back() == ' ')
    clean_str.pop_back();

  /* Next split the string into tokens */
  Tokens tokens;

  int last_space = -1;
  for (int i = 0; i < (int)clean_str.length(); i++) {
    char ch = clean_str[i];
    if (std::isspace(ch)) {
      assert(i - last_space - 1 >= 0);
      tokens.push_back(clean_str.substr(last_space + 1, i - last_space - 1));
      last_space = i;
    } else if (ch == '\"') {
      if (i > 1) {
        if (!std::isspace(clean_str[i - 1])) {
          log_message(ERR, "Syntax error");
          return {};
        }
      }
      i++;
      while (i < (int)clean_str.length()) {
        if (clean_str[i] == '\"') {
          if (i + 1 < (int)clean_str.length()) {
            if (!std::isspace(clean_str[i + 1])) {
              log_message(ERR, "Syntax error");
              return {};
            }
          }
          break;
        }
        i++;
      }
      if (i == (int)clean_str.length()) {
        log_message(ERR, "Syntax error");
        return {};
      } else
        tokens.push_back(clean_str.substr(last_space + 2, i - last_space - 2));
      i++;
      last_space = i;
    }
  }
  if (last_space + 1 <= (int)clean_str.length() - 1)
    tokens.push_back(clean_str.substr(last_space + 1));
  return tokens;
}

void
console_loop()
{
  if (_log_message_level <= INFO)
    std::cout << "Bomberbloke console...\n";
  while (!_halt) {
    std::string line;
    Tokens tokens;
    std::cout << ">";
    if (std::getline(std::cin, line)) {
      tokens = split_to_tokens(line);
      _command_queue.push(tokens);
    }
  }
  return;
}

std::shared_ptr<AbstractPlayer>
findPlayer(int id) {
  auto it = std::find_if(
    _player_list.begin(),_player_list.end(),
    [&](std::shared_ptr<AbstractPlayer> p) -> bool {
      return p->getId() == id;
    });

  if(it == _player_list.end())
    return {};

  return *it;
}


void
server_add_debug_player()
{
  auto player = std::make_shared<NetworkPlayer>("debug-bloke", -1);
  //.bool added = _net_server.addPlayer(player);
  _player_list.push_back(player);
  //if(!added) {
  //  log_message(ERR, "Requested debug player, but couldn't be added");
  //}
}

void
add_player(std::shared_ptr<AbstractPlayer> a_player)
{
  int id = _player_list.back()->getId();
  for (int i = 0; i < 1000; i++) {
    if (find_if(_player_list.begin(),
                _player_list.end(),
                [&](std::shared_ptr<AbstractPlayer> a_player) -> bool {
                  return a_player->getId() == id + i;
                }) == _player_list.end()) {
      a_player->setId(i + id);
      _player_list.push_back(a_player);
      return;
    }
  }
  log_message(ERR, "Couldn't add player! No free id");
}


void
handle_system_command_queue(IOSystem& ctx){
  while(_command_queue.size() > 0){
    auto tokens = _command_queue.pop();
    handle_system_command(ctx, tokens);
  }
}

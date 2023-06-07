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
#include "scene.hpp"
#include "syncEvent.hpp"
#include <SDL2/SDL_image.h>
#include <cereal/archives/json.hpp>
#include <dirent.h>
#include <exception>
#include <fstream>
#include <utility>
#include <cmrc/cmrc.hpp>

CMRC_DECLARE(files);



/*  TODO: reduce number of globals */
int _log_message_level = 0;
bool _bind_next_key = false;
std::string _next_bind_command;
int _window_size[] = { DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT };
SDL_Window* _window;
bool _halt = false;
std::list<LocalPlayer> _local_player_list;
SDL_Renderer* _renderer = NULL;
SDL_Joystick* _controller = nullptr;
bool _controller_connected = false;
int DEADZONE = 9000;
std::string dX = "0.1";
Uint8* _kb_state = NULL;
std::shared_ptr<scene> _pScene;
std::shared_ptr<scene> _pNewScene;
unsigned int _tick = 0;
std::string _nickname = "big_beef";
ServerInfo _server_info;
std::ofstream _console_log_file;
std::list<std::shared_ptr<AbstractSpriteHandler>> _particle_list;
std::vector<CommandBinding> _default_bindings;
std::list<std::shared_ptr<AbstractPlayer>> _player_list;
std::mutex _scene_mutex;

NetClient _net_client;
NetServer _net_server;

SoundManager soundManager;
TextManager textManager;

std::list<std::pair<std::string, SDL_Texture*>> _sprite_list;
static void
load_assets();

void
refresh_sprites();
void
create_window();

void
set_draw(bool on)
{
  if (on == _draw)
    return;

  else if (on == true) {
    _draw = true;
    create_window();
    refresh_sprites();
  }

  else {
    _draw = false;
    SDL_DestroyWindow(_window);
    SDL_DestroyRenderer(_renderer);
    _renderer = nullptr;
  }

  return;
}

void
exit_engine(int signum)
{
  set_draw(false);
  SDL_Delay(500);
  SDL_Quit();
  _halt = true;
  std::cout << "\nNow exiting the BLOKE engine. Hope you had fun. Wherever you "
               "are, we at the BLOKE project hope we have made your day just a "
               "little bit brighter. See you next time around! :)\n";
  signal(SIGINT, NULL);
#ifndef _WIN32
  std::cout << "Received signal " << strsignal(signum) << ".\nExiting...\n";
#endif
  SDL_Quit();
  return;
}

void
create_window()
{
  std::string window_name = "Bomberbloke Client";
  if (_server)
    window_name = "Bomberbloke Server";
  _window = SDL_CreateWindow(window_name.c_str(),
                             SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED,
                             _window_size[0],
                             _window_size[1],
                             SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if (_renderer) {
    SDL_DestroyRenderer(_renderer);
  }
  _renderer = SDL_CreateRenderer(_window, -1, 0);

  return;
}

/**  Refresh all of our sprites
 *
 *  This may be called when the window is resized.
 */

void
refresh_sprites()
{
  _pScene->refreshSprites();
  return;
}

void
resize_window(int x, int y)
{
  if (!_draw)
    return;

  _window_size[0] = x;
  _window_size[1] = y;

  if (_window) {
    SDL_SetWindowSize(_window, x, y);
  }
  if (_pScene) {
    _pScene->onResize();
    refresh_sprites();
  }
  return;
}

void
channelFinishedForwarder(int channel)
{
  soundManager.channelFinishedCallback(channel);
}

void
init_engine()
{
  signal(SIGINT, exit_engine);
  SDL_Init(SDL_INIT_EVERYTHING);
  soundManager.init(channelFinishedForwarder);
  load_assets();

  if (_draw) {
    create_window();
    if (_pScene)
      refresh_sprites();
  }

  /* Initialise the controller if it exists */
  _controller = handle_input_controller();
  _controller_connected = _controller != nullptr ? true : false;

  _kb_state = (Uint8*)malloc(sizeof(Uint8) * SDL_SCANCODE_APP2); // max scancode
  memset((void*)_kb_state, 0, sizeof(Uint8) * SDL_SCANCODE_APP2);

  std::thread console(console_loop);
  console.detach();
  /*  Open a log file  */
  _console_log_file.open("/tmp/bloke.log");

  return;
}

void
handle_input()
{
  SDL_Event event;
  //  bool key_up = true;
  Uint8* kb_state = NULL;
  while (SDL_PollEvent(&event)) {
    _pScene->onInput(&event);

    switch (event.type) {
      case SDL_QUIT: {
        _halt = true;
        break;
      }
      case SDL_KEYDOWN: {
        if (!_bind_next_key)
          break;
        /*We only look at keyboard events here in order to bind keys*/
        CommandBinding new_binding;
        new_binding.scancode = event.key.keysym.scancode;
        new_binding.command = _next_bind_command;
        _local_player_list.back().mControlScheme.push_back(new_binding);
        _bind_next_key = false;
        log_message(INFO,
                    "Successfully bound " + new_binding.command + " to " +
                      std::to_string(new_binding.scancode));
        break;
      }
      case SDL_WINDOWEVENT: {
        if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
          _window_size[0] = event.window.data1;
          _window_size[1] = event.window.data2;
          _pScene->onResize();
        }
      }
    }
  }

  kb_state = (Uint8*)SDL_GetKeyboardState(NULL);

  /*Iterate over local players */
  for (auto i = _local_player_list.begin(); i != _local_player_list.end();
       i++) {
    /*Iterate over key bindings */
    if (event.type != SDL_JOYAXISMOTION) {
      for (auto j = i->mControlScheme.begin(); j != i->mControlScheme.end();
           j++) {
        if (kb_state[j->scancode] !=
            _kb_state[j->scancode]) { // ensure that current keymap is different
                                      // to old
          // We will prepend "+" or "-" to the command depending on keystate
          std::string command_to_send =
            kb_state[j->scancode] ? "+" + j->command : "-" + j->command;

          log_message(DEBUG, j->command);

          if (std::find(_system_commands.begin(),
                        _system_commands.end(),
                        split_to_tokens(j->command).front()) != _system_commands.end()) {
            handle_system_command(
              split_to_tokens(command_to_send)); // process system command
          } else {
            std::shared_ptr<actor> character = i->getCharacter();
            if (character) {
              character->handleCommand(
                command_to_send); // handle normal command

              if (!_server) {
                std::unique_ptr<AbstractEvent> c_event(
                  new CommandEvent(command_to_send));
                _net_client.sendEvent(c_event);
              }
            } else {
              log_message(
                DEBUG, "Input received but no character connected to player!");
            }
          }
        }
      }
    } else {
      if (i->getCharacter() && event.jaxis.which == 0) {
        if (event.jaxis.axis == 0) { // x axis
          if (event.jaxis.value < -DEADZONE) {
            i->getCharacter()->handleCommand("left" + dX);
          } else if (event.jaxis.value > DEADZONE) {
            i->getCharacter()->handleCommand("+right" + dX);
          } else {
            i->getCharacter()->handleCommand("-XAxis" + dX);
          }
        } else if (event.jaxis.axis == 1) {
          if (event.jaxis.value < -DEADZONE) {
            i->getCharacter()->handleCommand("+up" + dX);
          } else if (event.jaxis.value > DEADZONE) {
            i->getCharacter()->handleCommand("+down" + dX);
          } else {
            i->getCharacter()->handleCommand("-YAxis" + dX);
          }
        }
      }

    }
  }

  // old key state, new key state
  memcpy(_kb_state, kb_state, sizeof(Uint8) * SDL_SCANCODE_APP2);
  return;
}

SDL_Joystick*
handle_input_controller()
{
  SDL_Init(SDL_INIT_JOYSTICK);
  if (SDL_NumJoysticks() > 0) {
    std::cout << "Controlled connected\n ";
    return SDL_JoystickOpen(0); // return joystick identifier
  } else
    return NULL; // no joystick found
}

void
draw_screen()
{
  if (_halt || !_renderer || !_window || !_draw)
    return;
  SDL_SetRenderDrawColor(_renderer, 0x00, 0x00, 0x00, 0xFF);
  SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_NONE);
  SDL_RenderClear(_renderer);
  if (_pScene)
    _pScene->draw();
  SDL_RenderPresent(_renderer);
  return;
}

void
logic_loop()
{
  return;
}

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
handle_system_command(std::list<std::string> tokens)
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
    _net_server.disconnectPlayer(*iter, tokens.back());
    return true;
  }

  else if (command == "players" && _server) {
    _net_server.printPlayers();
  }

  else if (command == "new" && _server) {
    log_message(INFO, "starting new game");
    new_game("");
    _net_server.syncPlayers();
  }

  else if (command == "zoom" && !_server && !key_down) {
    if (tokens.size() == 2) {
      std::string arg = tokens.back();

      auto camera = _pScene->getCamera();
      double zoom = DOUBLE_UNSET;

      if (camera)
        zoom = camera->GetZoom();

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
          camera->SetZoom(zoom);
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
      _net_client.disconnectClient();
      _pScene = std::make_shared<MainMenuScene>(15, 15);
    }
  }

  else if (command == "draw") {
    if (tokens.size() == 2) {
      if (tokens.back() == "on") {
        set_draw(true);
      } else if (tokens.back() == "off") {
        set_draw(false);
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

    else if (!_server && command == "open") {
      if (tokens.size() == 2) {
        /**
           NetClient::connectClient returns true or false. Return
           this value
        */
        std::string address = tokens.back();
        if (!_net_client.joinBlokeServer(address, _nickname)) {
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

      resize_window(x, y);
    } else {
      log_message(ERR, "Incorrect number of arguments for resize");
    }
  }

  else if (command == "bind") {
    if (tokens.size() == 3) {
      auto i = tokens.begin();
      i++;
      CommandBinding new_command;
      new_command.command = *i;
      i++;
      /*TODO: try catch*/
      new_command.scancode = SDL_Scancode(std::stoi(*i));

      _local_player_list.front().mControlScheme.push_back(new_command);
      log_message(INFO,
                  "Successfully bound command " + new_command.command + " to " +
                    std::to_string(new_command.scancode));
    } else if (tokens.size() == 2) {
      _bind_next_key = true;
      auto i = tokens.begin();
      i++;
      _next_bind_command = *i;
      log_message(INFO,
                  "binding next keypress to command: " + _next_bind_command);
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
      _net_client.sendEvent(c_event);
      log_message(INFO, "Requesting to change player colour to " + tokens.back());
      log_message(DEBUG, "Sending command \"" + command_to_send + "\"");
    }
  }
  else {
    log_message(ERR, "unknown command: " + command);
  }
  return true;
}

std::list<std::string>
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
  std::list<std::string> tokens;

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
  std::cout << "Bomberbloke console...\n";
  while (!_halt) {
    std::string line;
    std::list<std::string> tokens;
    std::cout << ">";
    if (std::getline(std::cin, line)) {
      tokens = split_to_tokens(line);
      handle_system_command(tokens);
    }
  }
  return;
}

/** This is used when the engine is started to pre load all assets into one
    place.
*/
static void
load_assets()
{

  auto fs = cmrc::files::get_filesystem();
  for (auto &&entry : fs.iterate_directory("files/assets/"))
  {
    auto dot_pos = entry.filename().find('.');
    if (dot_pos == std::string::npos)
    {
      continue;
    } // no file extension

    std::string file_name = entry.filename().substr(0, dot_pos);
    std::string file_extension = entry.filename().substr(dot_pos);

    auto file = fs.open("files/assets/" + entry.filename());
    SDL_RWops *io = SDL_RWFromConstMem(file.begin(), file.end() - file.begin());

    if (file_extension == ".ttf")
    {
      textManager.loadFontFromPath(io, file_name);
    }
    else if (file_extension == ".ogg")
    {
      soundManager.loadFromPath(io, file_name);
    }
    else if (file_extension == ".png"){
      SDL_Texture* sprite = IMG_LoadTexture_RW(_renderer, io, 1);
      _sprite_list.push_back({ entry.filename(), sprite });
    }
  }
}

/* Lookup the name in our list of assets and return a pointer to its texture if
 * it exists */
SDL_Texture*
get_sprite(std::string asset_name)
{
  auto iter =
    std::find_if(_sprite_list.begin(),
                 _sprite_list.end(),
                 [&](std::pair<std::string, SDL_Texture*> entry) -> bool {
                   return entry.first == asset_name;
                 });
  if (iter == _sprite_list.end()) {
    log_message(ERR, "Requested sprite, " + asset_name + " does not exist.");
    return nullptr;
  } else
    return iter->second;
}

void
add_player(std::shared_ptr<AbstractPlayer> a_player)
{
  unsigned int id = _player_list.back()->getId();
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

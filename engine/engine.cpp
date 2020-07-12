#include "engine.hpp"
#include <dirent.h>
#include "CommandEvent.hpp"
#include "syncEvent.hpp"
#include "QueryEvent.hpp"
#include "MoveEvent.hpp"
#include "ServerInfo.hpp"
#include "AbstractSpriteHandler.hpp"
#include "NetClient.hpp"
#include "NetServer.hpp"
#include "SoundManager.hpp"
#include <cereal/archives/json.hpp>
#include <fstream>
#include <exception>
#include "scene.hpp"
#include <utility>
#include <SDL2/SDL_image.h>

/*  TODO: reduce number of globals */
std::shared_ptr<Camera> _pCamera;
int _log_message_scene = 0;
bool _bind_next_key = false;
std::string _next_bind_command;
int _window_size[] = {DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT};
double _zoom = DEFAULT_ZOOM;
SDL_Window *_window;
bool _halt = false;
unsigned int _state;
std::list <LocalPlayer> _local_player_list;
std::list <std::shared_ptr<AbstractPlayer>> _player_list;
SDL_Renderer *_renderer = NULL;
SDL_Joystick *_controller = nullptr;
bool _controller_connected = false;
int DEADZONE = 9000;
std::string dX = "0.1";
Uint8 *_kb_state = NULL;
std::shared_ptr<scene> _pScene;
unsigned int _tick = 0;
std::string _nickname = "big_beef";
ServerInfo _server_info;
std::ofstream _console_log_file;
std::list<std::shared_ptr<AbstractSpriteHandler>> _particle_list;
std::vector<CommandBinding> _default_bindings;

NetClient _net_client;
NetServer _net_server;

SoundManager soundManager;

std::list<std::pair<std::string, SDL_Texture*>> _sprite_list;
static void load_assets();

void refresh_sprites();
void create_window();

void set_draw(bool on){
  if(on == _draw)
    return;

  else if(on == true){
    _draw = true;
    create_window();
    refresh_sprites();
  }

  else{
    _draw = false;
    SDL_DestroyWindow(_window);
    SDL_DestroyRenderer(_renderer);
    _renderer = nullptr;
  }

  return;
}

void exit_engine(int signum) {
    set_draw(false);
    SDL_Delay(500);
    SDL_Quit();
    _halt = true;
    std::cout  << "\nNow exiting the BLOKE engine. Hope you had fun. Wherever you are, we at the BLOKE project hope we have made your day just a little bit brighter. See you next time around! :)\n";
    signal(SIGINT, NULL);
    std::cout << "Received signal " << strsignal(signum) << ".\nExiting...\n";
    SDL_Quit();
    return;
}

void create_window(){
  std::string window_name = "Bomberbloke Client";
  if (_server)
    window_name = "Bomberbloke Server";
  _window = SDL_CreateWindow(window_name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                             _window_size[0], _window_size[1], SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if(_pCamera)
    _pCamera->SetZoom();
  // _zoom = (double)(_window_size[0]) / (_pScene->mDimmension[0]);
  if(_renderer){
    SDL_DestroyRenderer(_renderer);
  }
  _renderer = SDL_CreateRenderer(_window, -1, 0);
  SDL_SetRenderDrawColor(_renderer, 0xff, 0x00, 0x00, 0xff);
  SDL_RenderClear(_renderer);
  SDL_RenderPresent(_renderer);

  for(auto i = _sprite_list.begin(); i != _sprite_list.end(); i++){
    SDL_DestroyTexture(i->second);
  }

  return;
}

/**  Refresh all of our sprites
 *
 *  This may be called when the window is resized.
 */

void refresh_sprites(){
  _pScene->refreshSprites();
  return;
}


void resize_window(int x, int y){
  if(!_pCamera)
    return;

  _pCamera->SetZoom();
  _window_size[0] = x;
  _window_size[1] = y;

  if(_window){
    SDL_DestroyWindow(_window);
    create_window();
  }

    refresh_sprites();
  return;
}

void init_engine() {
  signal(SIGINT, exit_engine);
  SDL_Init(SDL_INIT_EVERYTHING);

    /*  Set blendmode */
    SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);

    if (_draw) {
      create_window();
    }

    _pScene = std::shared_ptr<scene>(new scene(10, 10));
    _pCamera = std::shared_ptr<Camera>(new Camera(_pScene));

    /* Initialise the controller if it exists */
    _controller = handle_input_controller();
    _controller_connected = _controller != nullptr ? true : false;

    _kb_state = (Uint8 *) malloc(sizeof(Uint8) * SDL_SCANCODE_APP2); //max scancode
    memset((void *) _kb_state, 0, sizeof(Uint8) * SDL_SCANCODE_APP2);
    std::thread console(console_loop);
    console.detach();
    _state = DISCONNECTED;
    /*  Open a log file  */
    _console_log_file.open("/tmp/bloke.log");

    load_assets();
    soundManager.init();
    
    // Play intro music
    soundManager.loadFromPath("assets/sounds/bomb_intro.wav");
    std::shared_ptr<Sound> pIntroSound = soundManager.createSound("bomb_intro");
    soundManager.playSound(*pIntroSound);
    
    return;
}

void handle_input() {
    SDL_Event event;
    //  bool key_up = true;
    Uint8 *kb_state = NULL;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
          _halt = true;
          break;
        case SDL_KEYDOWN:{
          if(!_bind_next_key)
            break;
          /*We only look at keyboard events here in order to bind keys*/
          CommandBinding new_binding;
          new_binding.scancode = event.key.keysym.scancode;
          new_binding.command  = _next_bind_command;
          _local_player_list.back().mControlScheme.push_back(new_binding);
          _bind_next_key = false;
          log_message(INFO, "Successfully bound " + new_binding.command + " to " + std::to_string(new_binding.scancode));
          break;
        }
        case SDL_WINDOWEVENT:
          if(event.window.event == SDL_WINDOWEVENT_RESIZED){
            _window_size[0] = event.window.data1;
            _window_size[1] = event.window.data2;
            _pCamera->SetZoom();
          }
        }
    }

    kb_state = (Uint8 *) SDL_GetKeyboardState(NULL);

    /*Iterate over local players */
    for (auto i = _local_player_list.begin(); i != _local_player_list.end(); i++) {
        /*Iterate over key bindings */
        if (event.type != SDL_JOYAXISMOTION) {
            for (auto j = i->mControlScheme.begin(); j != i->mControlScheme.end(); j++) {
                if (kb_state[j->scancode] != _kb_state[j->scancode]) { // ensure that current keymap is different to old
                    //We will prepend "+" or "-" to the command depending on keystate
                    std::string command_to_send = kb_state[j->scancode] ? "+" + j->command : "-" + j->command;
                    if (std::find(_system_commands.begin(), _system_commands.end(), j->command) !=
                        _system_commands.end()) {
                        handle_system_command(split_to_tokens(command_to_send)); // process system command
                    } else {
                      std::shared_ptr<actor> character = i->getCharacter();
                      if(character){
                        character->handle_command(command_to_send); // handle normal command
                        if(!_server){
                          std::unique_ptr<AbstractEvent> c_event(new CommandEvent(command_to_send));
                          _net_client.sendEvent(c_event);
                        }
                      }
                      else{
                        log_message(INFO, "No character connected to character");
                      }
                    }
                }
            }
        } else {
          if (i->getCharacter() &&  event.jaxis.which == 0) {
                if (event.jaxis.axis == 0) { //x axis
                    if (event.jaxis.value < -DEADZONE) {
                      i->getCharacter()->handle_command("left"+dX);
                    } else if (event.jaxis.value > DEADZONE) {
                      i->getCharacter()->handle_command("+right"+dX);

                    } else {
                      i->getCharacter()->handle_command("-XAxis"+dX);
                    }

                } else if (event.jaxis.axis == 1) {
                    if (event.jaxis.value < -DEADZONE) {
                      i->getCharacter()->handle_command("+up"+dX);
                    } else if (event.jaxis.value > DEADZONE) {
                      i->getCharacter()->handle_command("+down"+dX);
                    } else {
                      i->getCharacter()->handle_command("-YAxis"+dX);
                    }

                }
            }
        }

    }

    //old key state, new key state
    memcpy(_kb_state, kb_state, sizeof(Uint8) * SDL_SCANCODE_APP2);
    return;

}

SDL_Joystick *handle_input_controller() {
    SDL_Init(SDL_INIT_JOYSTICK);
    if (SDL_NumJoysticks() > 0) {
        std::cout << "Controlled connected\n ";
        return SDL_JoystickOpen(0); // return joystick identifier
    } else { return NULL; } // no joystick found
}

void draw_hud() {
    return;
}

void draw_screen() {

  if(_halt || !_renderer || !_window || !_draw)
    return;

  SDL_SetRenderDrawColor(_renderer, 0x10, 0xFF, 0x00, 0xFF);
  SDL_RenderClear(_renderer);
  _pCamera->draw();
  SDL_RenderPresent(_renderer);
  return;
}

void logic_loop() {
    return;
}

void log_message(int scene, std::string str) {
  if(scene > ALL)
    scene = ALL;

  /* Output to our log file */
  _console_log_file << str << "\n";

  if(scene < _log_message_scene){
    /*Ignore the message*/
    return;
  }
  else{
    std::cout << _tick << "\t " << LOG_LEVEL_STRINGS[scene] << ": " << str << std::endl;
    return;
  }
}

void load_config(std::string fname) {
    std::ifstream fin(fname, std::ifstream::in);
    char buf[32];
    int player_id = 0;
    bool in_player = false;
    if (!fin.good())
        return;
    while (fin.getline(buf, 32)) {
        std::string bind_string = std::string(buf);
        std::transform(bind_string.begin(), bind_string.end(), bind_string.begin(), ::tolower);
        if (in_player) {
            if (bind_string.find("end player")) {
                in_player = false;
                player_id++;
            }
        } else {
            if (bind_string.find("start player")) {
                in_player = true;
            }
        }
    }
}

bool handle_system_command(std::list<std::string> tokens) {

  if(tokens.size()==0)
    return true;

  std::string command = tokens.front();

  if(command == "new" && _server){
    for(auto i = _player_list.begin(); i != _player_list.end(); i++){
      std::unique_ptr<AbstractEvent> s_event(new syncEvent());
      ENetPeer* to = (*i)->getPeer();
      if(to)
        _net_server.sendEvent(s_event, to);
    }
  }

  if(command == "nickname" && !_server){
    if(tokens.size()!=2)
      log_message(ERROR, "nickname requires exactly one argument");
    else{
      _nickname = tokens.back();
      log_message(INFO, "nickname is now " + _nickname);
    }
  }

  if(command == "disconnect"){
    /*TODO:*/if(_server){

    }
    else{

    }
  }

  if(command == "draw"){
    if(tokens.size() == 2){
      if(tokens.back() == "on"){
        set_draw(true);
      }
      else if(tokens.back() == "off"){
        set_draw(false);
      }
      else{
        log_message(ERROR, "Couldn't parse command - " + command + tokens.back() + " the options for 'draw' are 'on' and 'off'");
        return false;
      }
    }
    else{
      log_message(ERROR, "draw command requires one argument");
      return false;
    }
  }

  if(_server && command == "new"){
    new_game("");
  }

  if(!_server && command == "open"){
    if(tokens.size() == 2){
      auto iter = tokens.begin();
      iter++;        //Select the first token
      int port;

      /*  Attempt to parse the first argument as address:port.
          If no ':' is present, the port number defaults to 8888.
       */
      long unsigned int delim_pos = iter->find(':');
      std::string address;
      if(delim_pos == std::string::npos){
        port = 8888;
        address = *iter;
      }
      else if(iter->substr(delim_pos+1).find(':')!=std::string::npos){
        std::stringstream msg;
        msg << "Couldn't parse address, " << *iter;
        log_message(ERROR, msg.str());
        return false;
      }
      else{
        address = iter->substr(0, delim_pos);
        /*  TODO replace try-catch with something less lazy to check if
            we're going to have an error
        */
        try{
          port = std::stoi(iter->substr(delim_pos+1).c_str());
        }
        catch(std::exception &e){
          log_message(ERROR, "Failed to parse port number");
          return false;
        }
      }
      /* We now have an address and port number to connect with
         NetClient::connectClient returns true of false. Return
         this value
       */
      if(!_net_client.joinBlokeServer(address, port, _nickname)){
        log_message(INFO, "failed to connect to server");
        return false;
      }
    }
    else{
      /* TODO allow port number as a separate argument? */
      log_message(ERROR, "Incorrect number of elements for connect");
    }
  }

  if(command == "info"){
    QueryEvent e("big_beef");
    cereal::JSONOutputArchive oArchive(std::cout);
    oArchive(e);
  }

  if(command == "log_level"){
    if(tokens.size()!=2){
      log_message(ERROR, "Command: log_level requires exactly one argument.");
      return false;
    }
    else{
      //Critical is our highest warning level
      auto iterator = tokens.begin();
      iterator++;
      std::string input_string = *iterator;
      for(unsigned int i = 0; i <= CRITICAL; i++){
        if(input_string == LOG_LEVEL_STRINGS[i]){
          _log_message_scene = i;
          log_message(ALL, "Log level set to " + LOG_LEVEL_STRINGS[i]);
        }
      }

    }
  }


  else if(command == "quit"){
    exit_engine(0);
  }

  else if(command ==  "generate_config"){
    if(tokens.size() == 1){
      GenerateConfig("generated_config.conf");
    }
    else if(tokens.size() == 2){
      std::string fname = tokens.back();
      GenerateConfig(fname);
    }
    else{
      log_message(ERROR, "To many arguments supplied to generate_config");
    }
  }

  else if(command == "resize"){
    if(tokens.size() == 3){
      auto i = tokens.begin();
      i++;
      /*TODO: Try catch here*/
      std::cout << *i << "\n";
      int x = std::stoi(*i);
      i++;
      int y = std::stoi(*i);

      resize_window(x,y);
    }
    else{
      log_message(ERROR, "Incorrect number of arguments for resize");
    }
  }

  else if(command == "bind"){
    if(tokens.size() == 3){
      auto i = tokens.begin();
      i++;
      CommandBinding new_command;
      new_command.command = *i;
      i++;
      /*TODO: try catch*/
      new_command.scancode = SDL_Scancode(std::stoi(*i));
      _local_player_list.front().mControlScheme.push_back(new_command);
      log_message(INFO, "Successfully bound command " +  new_command.command + " to " + std::to_string(new_command.scancode));
    }
    else if(tokens.size() == 2){
      _bind_next_key = true;
      auto i = tokens.begin();
      i++;
      _next_bind_command = *i;
      log_message(INFO, "binding next keypress to command: " + _next_bind_command);
    }

  }
  return true;
}

std::list <std::string> split_to_tokens(std::string str) {
    unsigned int last_index = 0;
    unsigned int count = 0;
    bool space = false;
    std::list <std::string> tokens;

    for (unsigned int i = 0; i < str.length(); i++) {
        char ch = str[i];
        if (space) {
            if (!std::isspace(ch)) {
                last_index = i;
                count = 1;
                space = false;
            }
        } else {
            if (std::isspace(ch)) {
                if (count != last_index) {
                    tokens.push_back(str.substr(last_index, count));
                    space = true;
                }
            } else
                count++;
        }
    }
    if (last_index != str.length() - 1){
      std::string last_token = str.substr(last_index);
      if(last_token.size()>0 && !std::isspace(last_token[0]))
        tokens.push_back(str.substr(last_index));
    }

    return tokens;
}

void console_loop() {
    std::cout << "Bomberbloke console...\n";
    while (!_halt) {
        switch (_state) {
            default: {
                std::string line;
                std::list <std::string> tokens;

                std::cout << ">";
                if(std::getline(std::cin, line)){
                    tokens = split_to_tokens(line);
                    handle_system_command(tokens);
                  }
                break;
            }
            case JOINING:
                break;
        }
    }
    return;
}

/** This is used when the engine is started to pre load all assets into one
    place.

    TODO: handle files other than images.
*/
static void load_assets(){
  if (auto dir = opendir(("assets" + PATHSEPARATOR).c_str())){
    while (auto f = readdir(dir)) {
      if (f->d_name[0] == '.')
        continue; // Skip hidden files
      else{
        std::string whole_filename = std::string(f->d_name);
        auto dot_pos = whole_filename.find('.');
        if(dot_pos == std::string::npos)
          continue; // no file extension
        std::string file_name = whole_filename.substr(0, dot_pos);
        std::string file_extension = whole_filename.substr(dot_pos);
        if(file_extension == ".png"){
          SDL_Texture *sprite = IMG_LoadTexture(_renderer, ("assets" + PATHSEPARATOR +  whole_filename).c_str());
          _sprite_list.push_back({whole_filename, sprite});
        }
      }
    }
    closedir(dir);
  }
}

/* Lookup the name in our list of assets and return a pointer to its texture if it exists */
SDL_Texture *get_sprite(std::string asset_name){
  auto iter = std::find_if(_sprite_list.begin(), _sprite_list.end(), [&](std::pair<std::string, SDL_Texture*> entry) -> bool{return entry.first==asset_name;});
  if(iter == _sprite_list.end()){
    log_message(ERROR, "Requested sprite, " + asset_name + " does not exist.");
    return nullptr;
  }
  else
    return iter->second;
}

void add_player(std::shared_ptr<AbstractPlayer> a_player){
  int id = _player_list.back()->getId();
  for(int i = 0; i < 1000;i++){
    if(find_if(_player_list.begin(), _player_list.end(), [&](std::shared_ptr<AbstractPlayer> a_player) -> bool {return a_player->getId() == id + i;}) == _player_list.end()){
      a_player->setId(i + id);
      _player_list.push_back(a_player);
      return;
    }
  }
  log_message(ERROR, "Couldn't add player! No free id");
}

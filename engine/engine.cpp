#include "engine.hpp"
#include "QueryEvent.hpp"
#include "MoveEvent.hpp"
#include "ServerInfo.hpp"
#include "AbstractSpriteHandler.hpp"
#include <cereal/archives/json.hpp>
#include <fstream>

/*  Globals */
int _log_message_level = 0;
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
level _level;
unsigned int _tick = 0;
std::string _nickname = "big_beef";
ServerInfo _server_info;
std::ofstream _console_log_file;
std::list<std::shared_ptr<AbstractSpriteHandler>> _particle_list;
std::vector<CommandBinding> _default_bindings;


void exit_engine(int signum) {
    //Destroy window
    // // if (_window) {
    // //   //     SDL_DestroyWindow(_window);
    //     _window = NULL;
    // }
    //Quit SDL subsystems
    SDL_Delay(500);
    SDL_Quit();
    _halt = true;
    std::cout  << "\nNow exiting the BLOKE engine. Hope you had fun. Wherever you are, we at the BLOKE project hope we have made your day just a little bit brighter. See you next time around! :)\n";
    signal(SIGINT, NULL);
    std::cout << "Received signal " << strsignal(signum) << ".\nExiting...\n";
    return;
}
void create_window(){
  std::string window_name = "Bomberbloke Client";
  if (_server)
    window_name = "Bomberbloke Server";
  _window = SDL_CreateWindow(window_name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                             _window_size[0], _window_size[1], SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  _zoom = (double)(_window_size[0]) / (_level.mDimmension[0]);
  if(_renderer){
    SDL_DestroyRenderer(_renderer);
  }
  _renderer = SDL_CreateRenderer(_window, -1, 0);
  SDL_SetRenderDrawColor(_renderer, 0xff, 0x00, 0x00, 0xff);
  SDL_RenderClear(_renderer);
  SDL_RenderPresent(_renderer);
}

/*  Reload all of our sprites */
void ReloadSprites(){
  _level.ReloadSprites();
  for(auto i = _particle_list.begin(); i != _particle_list.end(); i++){
    (*i)->ReloadSprite();
  }
  return;
}


void resize_window(int x, int y){
  _window_size[0] = x;
  _window_size[1] = y;

  if(_window){
    SDL_DestroyWindow(_window);
    create_window();
  }
  ReloadSprites();
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
   _level = level();

    // init controller
    _controller = handle_input_controller();
    _controller_connected = _controller != nullptr ? true : false;

    _kb_state = (Uint8 *) malloc(sizeof(Uint8) * SDL_SCANCODE_APP2); //max scancode
    memset((void *) _kb_state, 0, sizeof(Uint8) * SDL_SCANCODE_APP2);
    std::thread console(console_loop);
    console.detach();
    _state = DISCONNECTED;
    /*  Open a log file  */
    _console_log_file.open("/tmp/bloke.log");

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
            _zoom = (double)(_window_size[0]) / (_level.mDimmension[0]);
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
                      std::cout << command_to_send << "\n";
                      std::shared_ptr<actor> character = i->getCharacter();
                      if(i->getCharacter())
                        i->getCharacter()->handle_command(command_to_send); // handle normal command
                      else{
                        log_message(INFO, "No character connected to character");
                      }
                    }
                }
            }
        } else {
            if (event.jaxis.which == 0) {
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


bool handle_collision(std::shared_ptr<actor> a, std::shared_ptr<actor> b) {
    bool collision = true;

    std::vector<double> aPosition = {a->mPosition[0], a->mPosition[1]};
    std::vector<double> bPosition = {b->mPosition[0], b->mPosition[1]};

    // Test axes normal to actor a
    // TODO: Move test axis generation into ColliderFrame
    int aNVertices =  a->mColliderFrame.mFrameVertices.size();
    for (int i = 0; i < aNVertices - 1; i++) {
        // Normal axis is given (-y, x) where (x, y) = v2 - v1
        std::vector<double> axis = {
            a->mColliderFrame.mFrameVertices[i][1] - a->mColliderFrame.mFrameVertices[i + 1][1],
            a->mColliderFrame.mFrameVertices[i + 1][0] - a->mColliderFrame.mFrameVertices[i][0]
        };

        std::pair<double, double> aProjVal = a->mColliderFrame.projectOntoAxis(axis, aPosition);
        std::pair<double, double> bProjVal = b->mColliderFrame.projectOntoAxis(axis, bPosition);

        if (aProjVal.second < bProjVal.first || aProjVal.first > bProjVal.second)
            collision = false;
    }

    // Test axes normal to actor b
    int bNVertices =  b->mColliderFrame.mFrameVertices.size();
    for (int i = 0; i < bNVertices - 1; i++) {
        // Normal axis is given (-y, x) where (x, y) = v2 - v1
        std::vector<double> axis = {
            b->mColliderFrame.mFrameVertices[i][1] - a->mColliderFrame.mFrameVertices[i + 1][1],
            b->mColliderFrame.mFrameVertices[i + 1][0] - a->mColliderFrame.mFrameVertices[i][0]
        };

        std::pair<double, double> aProjVal = a->mColliderFrame.projectOntoAxis(axis, aPosition);
        std::pair<double, double> bProjVal = b->mColliderFrame.projectOntoAxis(axis, bPosition);

        if (aProjVal.second < bProjVal.first || aProjVal.first > bProjVal.second)
            collision = false;
    }
    return collision;
}

void draw_hud() {
    return;
}

void draw_screen() {
  if(_halt)
    return;
  SDL_SetRenderDrawColor(_renderer, 0x10, 0xFF, 0x00, 0xFF);
  SDL_RenderClear(_renderer);

  _level.draw();

  _level.cleanUp();

  SDL_RenderPresent(_renderer);
  return;
}

void logic_loop() {
    return;
}

void log_message(int level, std::string str) {
  if(level > ALL)
    level = ALL;

  /* Output to our log file */
  _console_log_file << str << "\n";

  if(level < _log_message_level){
    /*Ignore the message*/
    return;
  }
  else{
    std::cout << LOG_LEVEL_STRINGS[level] << ": " << str << std::endl;
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

// void parseServerIP(std::string ip){
//   //potentially has : for port. we need to split this strng
//   size_t found = ip.find_first_of(":");
  
//   if (found != std::string::npos){
//     setAddress(ip.substr(0,found),std::stoi((ip.substr(found))));
//   }
//   else{
//     // assume default port
//     setAddress(ip);
//   }
// }

bool handle_system_command(std::list<std::string> tokens) {
  std::string command = tokens.front();

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
          _log_message_level = i;
          log_message(ALL, "Log level set to " + LOG_LEVEL_STRINGS[i]);
        }
      }

    }
  }


  else if(command == "quit"){
    exit_engine(0);
  }

  // if(command == "connect"){
  //   if (tokens.size() == 2){
  //     parseServerIP(std::string(tokens.back()));
  //   } else{
  //     setAddress();
  //   }
  // }

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
    std::cout << "Bomberbloke console\n";
    while (!_halt) {
        switch (_state) {
            default: {
                std::string line;
                std::list <std::string> tokens;

                std::cout << ">";
                std::getline(std::cin, line);
                tokens = split_to_tokens(line);
                handle_system_command(tokens);
                break;
            }
            case JOINING:
                break;
        }
    }
    return;
}

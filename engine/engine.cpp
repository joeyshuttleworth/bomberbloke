#include "engine.h"
#include "QueryEvent.hpp"
#include "MoveEvent.hpp"
#include "ServerInfo.hpp"
#include <cereal/archives/json.hpp>
#include <fstream>

bool _bind_next_key = false;
std::string _next_bind_command = "";
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

void resize_window(int x, int y){
  _window_size[0] = x;
  _window_size[1] = y;

  if(_window){
    SDL_DestroyWindow(_window);
    create_window();
  }
  _level.ReloadSprites();
  return;
}

static int resizeWatcher(void *data, SDL_Event *event){
  if(event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_RESIZED)
    resize_window(event->window.data1, event->window.data2);
}

void init_engine() {
    int size[2];
    char *receive_port = (char *) "8888";

    _server_info;
    signal(SIGINT, exit_engine);
    SDL_Init(SDL_INIT_EVERYTHING);

    if (_draw) {
      create_window();
    }
   _level = level();

    // init controller
    _controller = handle_input_controller();
    _controller_connected = _controller != NULL ? true : false;

    _kb_state = (Uint8 *) malloc(sizeof(Uint8) * SDL_SCANCODE_APP2); //max scancode
    memset((void *) _kb_state, 0, sizeof(Uint8) * SDL_SCANCODE_APP2);
    std::thread console(console_loop);
    console.detach();
    _state = DISCONNECTED;

    SDL_AddEventWatch(resizeWatcher, _window);
    return;
}

void handle_input(level *level) {
    SDL_Event event;
    //  bool key_up = true;
    Uint8 *kb_state = NULL;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
          _halt = true;
          break;
        case SDL_KEYDOWN:{
          /*We only look at keyboard events here in order to bind keys*/
          CommandBinding new_binding;
          new_binding.scancode = event.key.keysym.scancode;
          new_binding.command  = _next_bind_command;
          _local_player_list.back().mControlScheme.push_back(new_binding);
          _bind_next_key = false;
          log_message(INFO, "Successfully bound " + new_binding.command + " to " + std::to_string(new_binding.scancode));
          break;
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
                      i->getCharacter()->handle_command(command_to_send); // handle normal command
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
    double a_tmp_midpoint[2] = {a->get_midpoint(0), a->get_midpoint(1)}; 
    double b_midpoint[2] = {b->get_midpoint(0), b->get_midpoint(1)};
    double distance[2];
    if (!a->is_moving())
        return false;
    for (int i = 0; i < 2; i++) {
        a_tmp_midpoint[i] = a_tmp_midpoint[i] + a->mVelocity[i];
        if ((distance[i] = std::abs(a_tmp_midpoint[i] - b_midpoint[i])) >
            double(0.5 * (a->mDimmension[i] + b->mDimmension[i]))) {
            collision = false;
            break;
        }
    }
    if (collision) {
        int x_iter = 1;
        int i = 0;
        if (distance[1] > distance[0])
            i = 1;
        if (a->mVelocity[i] < 0)
            x_iter = -1;
        a_tmp_midpoint[i] = a_tmp_midpoint[i] - x_iter * (1e-8 + (a->mDimmension[i] + b->mDimmension[i]) * 0.5 -
                                                std::abs(a_tmp_midpoint[i] - b_midpoint[i]));
        a->mVelocity[i] = 0;
        a->move(a_tmp_midpoint[0] - a->mDimmension[0] / 2, a_tmp_midpoint[1] - a->mDimmension[1] / 2);
    }
    return collision;
}

void handle_movement() {
    /*Iterate over all moving actors*/
    for (auto i = _level.mActors.begin(); i != _level.mActors.end(); i++) {
        bool collision = false;

        /*Update actors*/

        (*i)->update();

        if (!(*i)->is_moving())
            continue;
        if ((*i)->mCollides == false)
            (*i)->move((*i)->mPosition[0] + (*i)->mVelocity[0], (*i)->mPosition[1] + (*i)->mVelocity[1]);

        else {

            /*Check for collisions*/

            for (auto j = _level.mActors.begin(); j != _level.mActors.end(); j++) {
                if (i == j)
                    continue;
                if ((*j)->mCollides == true)
                    if (handle_collision(*i, *j)) {
                        collision = true;
                        break;
                    }
                if (collision)
                    break;
            }
            if (!collision)
                (*i)->move((*i)->mPosition[0] + (*i)->mVelocity[0], (*i)->mPosition[1] + (*i)->mVelocity[1]);
        }
    }
    return;
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
  SDL_RenderPresent(_renderer);
  return;
}

void logic_loop() {
    return;
}

void log_message(int level, std::string str) {
    std::cout << level << str << std::endl;
    return;
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
  std::string command = tokens.front();

  if(command == "info"){
    QueryEvent e("big_beef");
    cereal::JSONOutputArchive oArchive(std::cout);
    oArchive(e);
  }

  if(command ==  "generate_config"){
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

  if(command == "resize"){
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

  if(command == "bind"){
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

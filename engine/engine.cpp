#include "engine.h"

double _zoom = DEFAULT_ZOOM;
SDL_Window *_window;
bool _halt = false;
unsigned int _state;
std::list <localPlayer> _local_player_list;
std::list <networkPlayer> _client_list;
pthread_t net_receive, read_console;
SDL_Renderer *_renderer = NULL;

Uint8 *_kb_state = NULL;
level _level;
uint32_t _tick = 0;
std::string _nickname = "big_beef";

void exit_engine(int signum) {
    //Destroy window
    if (_window) {
        SDL_DestroyWindow(_window);
        _window = NULL;
    }
    //Quit SDL subsystems
    SDL_Quit();
    _halt = true;
    std::cout
            << "\nNow exiting the BLOKE engine. Hope you had fun. Wherever you are, we at the BLOKE project hope we have made your day just a little bit brighter. See you next time around! :)\n";
    //  net_exit();
    signal(SIGINT, NULL);
    return;
}

void init_engine() {
    int size[2];
    char *receive_port = (char *) "8888";

    signal(SIGINT, exit_engine);
    SDL_Init(SDL_INIT_EVERYTHING);
    if (_draw) {
        std::string window_name = "Bomberbloke Client";
        if (_server)
            window_name = "Bomberbloke Server";
        _window = SDL_CreateWindow(window_name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                   DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
        SDL_GetWindowSize(_window, size, size + sizeof(int));
        _zoom = size[0] / (_level.mDimmension[0]);
    }
    _renderer = SDL_CreateRenderer(_window, -1, 0);
    SDL_SetRenderDrawColor(_renderer, 0xff, 0xff, 0, 0xff);
    SDL_RenderClear(_renderer);
    SDL_RenderPresent(_renderer);
    _level = level();

    // init controller
    _controller = handle_input_controller();
    _controller_connected = _controller != NULL ? true: false;

    _kb_state = (Uint8 *) malloc(sizeof(Uint8) * SDL_SCANCODE_APP2); //max scancode
    memset((void *) _kb_state, 0, sizeof(Uint8) * SDL_SCANCODE_APP2);
    pthread_create(&read_console, NULL, console_loop, NULL);
    _state = DISCONNECTED;
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
        }
    }
    kb_state = (Uint8 *) SDL_GetKeyboardState(NULL);

    /*Iterate over local players */
    for (auto i = _local_player_list.begin(); i != _local_player_list.end(); i++) {
        /*Iterate over key bindings */
      for (auto j = i->mControlScheme.begin(); j != i->mControlScheme.end(); j++) {
            if (kb_state[j->scancode] != _kb_state[j->scancode]) { // ensure that current keymap is different to old
	        //We will prepend "+" or "-" to the command depending on keystate
         	std::string command_to_send = kb_state[j->scancode]? "+" +  j->command : "-" + j->command;
                if (std::find(_system_commands.begin(), _system_commands.end(), j->command) != _system_commands.end()) {
                    handle_system_command(split_to_tokens(command_to_send)); // process system command
                } else{
		  i->character->handle_command(command_to_send); // handle normal command  
		}
            }
        }
    }
    //old key state, new key state
    memcpy(_kb_state, kb_state, sizeof(Uint8) * SDL_SCANCODE_APP2);
    return;
}

SDL_Joystick* handle_input_controller() {
    SDL_Init(SDL_INIT_JOYSTICK);

    if (SDL_NumJoysticks() > 0) {
        return SDL_JoystickOpen(0); // return joystick identifier
    } else { return NULL; } // no joystick found
}




bool handle_collision(actor *a, actor *b) {
    bool collision = true;
    double *a_tmp_pos;
    double *b_midpoint;
    double distance[2];
    if (!a->is_moving())
        return false;
    a_tmp_pos = a->get_midpoint();
    b_midpoint = b->get_midpoint();
    for (int i = 0; i < 2; i++) {
        a_tmp_pos[i] = a_tmp_pos[i] + a->mVelocity[i];
        if ((distance[i] = std::abs(a_tmp_pos[i] - b_midpoint[i])) >
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
        a_tmp_pos[i] = a_tmp_pos[i] - x_iter * (1e-8 + (a->mDimmension[i] + b->mDimmension[i]) * 0.5 -
                                                std::abs(a_tmp_pos[i] - b_midpoint[i]));
        a->mVelocity[i] = 0;
        a->move(a_tmp_pos[0] - a->mDimmension[0] / 2, a_tmp_pos[1] - a->mDimmension[1] / 2);
    }
    free(a_tmp_pos);
    free(b_midpoint);
    return collision;
}

void handle_movement() {
    /*Iterate over all moving actors*/
    for (auto i = _level.actorList.begin(); i != _level.actorList.end(); i++) {
        bool collision = false;

        /*Update actors*/

        (*i)->update();

        if (!(*i)->is_moving())
            continue;
        if ((*i)->mCollides == false)
            (*i)->move((*i)->mPosition[0] + (*i)->mVelocity[0], (*i)->mPosition[1] + (*i)->mVelocity[1]);

        else {

            /*Check for collisions*/

            for (auto j = _level.actorList.begin(); j != _level.actorList.end(); j++) {
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
    SDL_SetRenderDrawColor(_renderer, 0x10, 0xFF, 0x00, 0xFF);
    SDL_RenderClear(_renderer);
    _level.draw();
    SDL_RenderPresent(_renderer);
    return;
}

void logic_loop() {
    return;
}

void log_message(int level, const char *string) {
    std::cout << level << std::string(string) << std::endl;
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

void handle_system_command(std::list <std::string> tokens) {
    return;
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
    if (last_index != str.length() - 1)
        tokens.push_back(str.substr(last_index));
    return tokens;
}

void *console_loop(void *arg) {
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
    return NULL;
}


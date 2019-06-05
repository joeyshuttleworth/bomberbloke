#include "engine.h"

double _zoom = DEFAULT_ZOOM;
SDL_Window  *_window;
SDL_Surface *_surface;
bool         _halt= false;
unsigned int _state;
std::list<local_p> _local_player_list;
std::list<network_p> _client_list;
pthread_t net_receive, read_console;
SDL_Renderer *_renderer = NULL;

Uint8 *_kb_state = NULL;
level _level;
uint32_t  _tick = 0;
std::string _nickname = "big_beef";

void exit_engine(int signum){
   //Destroy window
  if(_window){
    SDL_DestroyWindow(_window);
    _window = NULL;
  }
  //Quit SDL subsystems
  SDL_Quit();
  _halt = true;
  std::cout << "\nNow exiting the BLOKE engine. Hope you had fun. Wherever you are, we at the BLOKE project hope we have made your day just a little bit brighter. See you next time around! :)\n";
  net_exit();
  signal(SIGINT, NULL);
  return;
}
void init_engine(){
  int size[2];
  char *receive_port = (char*)"8888";

  signal(SIGINT, exit_engine);
  SDL_Init(SDL_INIT_EVERYTHING);
  if(_draw){
    std::string window_name = "Bomberbloke Client";
    if(_server)
      window_name = "Bomberbloke Server";
    _window = SDL_CreateWindow(window_name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_GetWindowSize(_window, size, size+sizeof(int));
    _zoom=size[0]/(_level.dim[0]);
  }
  _renderer = SDL_CreateRenderer(_window, -1, 0);
  SDL_SetRenderDrawColor(_renderer, 0xff, 0xff, 0, 0xff);
  SDL_RenderClear(_renderer);
  SDL_RenderPresent(_renderer);
  _level = level(10,10);
  _kb_state = (Uint8*)malloc(sizeof(Uint8) * SDL_SCANCODE_APP2); //max scancode
  memset((void*)_kb_state, 0, sizeof(Uint8) * SDL_SCANCODE_APP2);
  _level.init();
  net_messages_init();
  if(!_server){
    memset(&_server_address, 0, sizeof(_server_address));
    receive_port = NULL;
  }
  if(_server)
    pthread_create(&net_receive,  NULL, receive_loop, (void*) receive_port);
  else
    pthread_create(&net_receive,  NULL, receive_loop, NULL);
  pthread_create(&read_console, NULL, console_loop, NULL); 
  _state = DISCONNECTED;
  return;
}

void handle_input(level *level){
  SDL_Event event;
  //  bool key_up = true;
  Uint8 *kb_state = NULL;
  while(SDL_PollEvent(&event)){
    switch(event.type){
    case SDL_QUIT:
      _halt = true;
      break;
    }
  }
  kb_state = (Uint8*)SDL_GetKeyboardState(NULL);
  for(auto i = _local_player_list.begin(); i != _local_player_list.end(); i++){
    bool no_command = true;
    for(auto j = i->control_scheme.begin(); j!=i->control_scheme.end(); j++){
      if(kb_state[j->scancode]){
	if(std::find(_system_commands.begin(), _system_commands.end(), j->command)!=_system_commands.end()){
	  handle_system_command(split_to_tokens(j->command));
	}
	else
	  i->character->handle_command(j->command);
      }
    }
    if(no_command && i->character!=NULL)
      i->character->handle_command(std::string(""));
  }
  memcpy(_kb_state, kb_state, sizeof(Uint8) * SDL_SCANCODE_APP2); 
  return;
}

bool handle_collision(actor *a, actor *b){
  bool collision = true;
  double *a_tmp_pos;
  double *b_midpoint;
  double distance[2];
  if(!a->is_moving())
    return false; 
  a_tmp_pos  =  a->get_midpoint();
  b_midpoint =  b->get_midpoint();
  for(int i = 0; i < 2; i++){
    a_tmp_pos[i] = a_tmp_pos[i] + a->velocity[i];
    if((distance[i]=std::abs(a_tmp_pos[i] - b_midpoint[i]))> double(0.5 * (a->dim[i] + b->dim[i]))){
      collision = false;
      break;
    }
  }
  if(collision){
    int x_iter = 1;
    int i = 0;
    if(distance[1]>distance[0])
      i = 1;
    if (a->velocity[i]<0)
      x_iter=-1;
    a_tmp_pos[i] = a_tmp_pos[i]-x_iter * (0.0000000000001 + (a->dim[i] + b->dim[i]) * 0.5 - std::abs(a_tmp_pos[i] - b_midpoint[i]));
      a->velocity[i] = 0;
      a->move(a_tmp_pos[0]-a->dim[0]/2, a_tmp_pos[1]-a->dim[1]/2);
  }
  free(a_tmp_pos);
  free(b_midpoint);
  return collision;
}

void handle_movement(){
  std::list<net_move> move_list;
  for(auto i = _level.actor_list.begin(); i!=_level.actor_list.end(); i++){
    bool collision = false;
    if(!i->is_moving())
      continue;
    if((i)->collides==false)
      (i)->move((i)->position[0]+(i)->velocity[0], (i)->position[1] + (i)->velocity[1]);
    else{
      for(auto j = _level.actor_list.begin(); j!=_level.actor_list.end(); j++){
	if(i==j)
	  continue;
      if(j->collides==true)
        if(handle_collision(&(*i), &(*j))){
	  collision=true;
	  break;
	}
      if(collision)
	break;
      }
      if(!collision)
	(i)->move((i)->position[0]+(i)->velocity[0], (i)->position[1] + (i)->velocity[1]);
      if(_server)
      if ((i)->velocity[0]<MIN_VELOCITY)
	(i)->velocity[0] = 0;
      if((i)->velocity[1]<MIN_VELOCITY)
	(i)->velocity[1] = 0;
    }
    if(_server && (_tick & NET_RATE) == 0){
      net_move move;
      move.id = (i)->id;
      net_float_create(i->position[0], &(move.position[0]));
      net_float_create(i->position[1], &(move.position[1]));
      net_float_create(i->velocity[0], &(move.velocity[0]));
      net_float_create(i->velocity[1], &(move.velocity[1]));      
      move_list.push_back(move);
    }
  }
  return;
}

void draw_hud(){
  return;
}

void draw_screen(){
  SDL_SetRenderDrawColor(_renderer, 0, 0x00, 0xFF, 0xFF);
  SDL_RenderClear(_renderer);
  _level.draw();
  draw_hud();
  SDL_RenderPresent(_renderer);
  return;
}

void logic_loop(){
  return;
}

void log_message(int level, const char *string){
  std::cout << level << std::string(string) << std::endl;
  return;
}

void load_config(std::string fname){
  std::ifstream fin(fname, std::ifstream::in);
  char buf[32];
  int player_id = 0;
  bool in_player = false;
  if(!fin.good())
    return;
  while(fin.getline(buf, 32)){
    std::string bind_string = std::string(buf);
    std::transform(bind_string.begin(), bind_string.end(), bind_string.begin(), ::tolower);
    if(in_player){
      if(bind_string.find("end player")){
	in_player = false;
	player_id++;
      }
    }
    else{
      if(bind_string.find("start player")){
	in_player = true;
      }
    }
  }
}

void handle_system_command(std::list<std::string> tokens){
  std::string command = tokens.front();
  struct addrinfo hints;
  char buf[512];
  tokens.pop_front();  //command no longer at the front of tokens
  if(_halt)
    return;
  std::cout << command << "\n";
  if(command == "exit"){
    exit_engine(0);
  }
  else if(command == "connect" && !_server){
    const char *address = NULL;
    const char *port    = "8888";
    if(tokens.size() > 0){
      const char *address = tokens.begin()->c_str();
      if(tokens.size() > 1){
	tokens.pop_front();
	const char *port    = tokens.begin()->c_str();
      }
    }
    net_join_server(address, port, _nickname.c_str());
  }
  else if(command == "nickname" && !_server){
    if(tokens.size() == 1){
      _nickname = tokens.front();
      log_message(INFO, ("nickname has been set to: " + _nickname + "\n").c_str());
    }
    else
      log_message(ERROR, "Couldn't reset nickname. Incorrect number of arguments. \n");
  }
  else if(_server && command == "pause"){
    _state = PAUSED;
  }
  else if(_server && command == "start"){
    engine_start_game();
    _state = PLAYING;    
  }
  else if(command == "disconnect" && !_server && _state!=DISCONNECTED){
    net_message msg;
    net_clear_messages();
    msg.operation = NET_LEAVE;
    msg.address = _server_address;
    msg.address_length = 0;
    msg.attempts = 1;
    msg.frequency = 0;
    msg.data_size = 0;
    msg.data = NULL;
    std::cout << "Disconnecting from server\n";
    _state = DISCONNECTED;
    net_add_message(&msg, false);
  }
  else if(_server && command == "unpause"){
    if(_state == PAUSED)
      _state = PLAYING;
  }
  else if(_server && command == "new"){
    net_message msg;
    msg.operation = NET_NEW_GAME;
    msg.data_size = sizeof(_tick);
    msg.data = (char*)&_tick;
    msg.frequency = 1;
    msg.attempts = 20;
    send_to_list(&msg, _client_list);    
    engine_new_game(""); // pass tokens as a parameter
  }
  else if(_server && command == "stop"){
    if(_state == PLAYING)
      _state = STOPPED;
  }
  else if(command == "say"){
    if(tokens.size() >= 1 && _state != DISCONNECTED){
      std::string str;
      net_message msg;
      for(auto i = tokens.begin(); i!=tokens.end();i++){
	str.append(*i + " ");
      }
      msg.data_size = str.length()+1;
      msg.attempts = 10;
      msg.frequency = TICK_RATE;
      msg.operation = NET_MSG;
      if(!_server){
	msg.data = (char*)malloc(str.length());
	strncpy(msg.data, str.c_str(), str.length()+1);
	msg.address = _server_address;
	msg.address_length = sizeof(struct sockaddr_in);
	net_add_message(&msg, false);
      }
      else{
	str = "server: " + str;
	msg.data = (char*)malloc(str.length()+1);
	strncpy(msg.data, str.c_str(), str.length());
	msg.data_size = str.length();
	send_to_list(&msg, _client_list);
      }
      free(msg.data);
    }
  }
  else{
    std::cout << "Command not recognised\n";
  }
  return;
}

std::list<std::string> split_to_tokens(std::string str){
  unsigned int last_index = 0;
  unsigned int count = 0;
  bool space = false;
  std::list<std::string> tokens;

  for(unsigned int i = 0; i < str.length(); i++){
    char ch = str[i];
    if(space){
      if(!std::isspace(ch)){
	last_index=i;
	count=1;
	space=false;
      }
    }
    else{
      if(std::isspace(ch)){
	if(count!=last_index){
	  tokens.push_back(str.substr(last_index, count));
	  space = true;
	}
      }
      else
	count++;
    }
  }
  if(last_index != str.length() - 1)
    tokens.push_back(str.substr(last_index));
  return tokens;
}

void *console_loop(void *arg){
  std::cout << "Bomberbloke console\n";
  while(!_halt){
    switch (_state){
	default:{
	  std::string line;
	  std::list<std::string> tokens;
	  
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

char *write_move(net_move move){
  char *rc;
  //TODO whatever this was supposed to be
  return rc;
}

void send_to_list(net_message *msg, std::list<network_p> lst){
  for(auto i = lst.begin(); i!= lst.end(); i++){
    memcpy(&msg->address, i->address, i->address_length);
    msg->address_length = i->address_length;
    net_add_message(msg, false);
  }
  return;
}

#include "engine.h"
#include "level.h"
#include "actor.h"
#include "player.h"

extern "C" {
#include "net.h"
}

double _zoom = DEFAULT_ZOOM;
SDL_Window  *_window;
SDL_Surface *_surface;
bool         _exit= false;
std::list<local_p> _local_player_list;

Uint8 *_kb_state = NULL;

void init_engine(level *level){
  int *size = (int*)malloc(2*sizeof(int));
  _window = SDL_CreateWindow("Bomberbloke", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
  _surface=SDL_GetWindowSurface(_window);
  SDL_GetWindowSize(_window, size, size+sizeof(int));
  _zoom=size[0]/level->dim[0];
  _kb_state = (Uint8*)malloc(sizeof(Uint8) * SDL_SCANCODE_APP2); //max scancode
  memset((void*)_kb_state, 0, sizeof(Uint8) * SDL_SCANCODE_APP2);
  level->init();
  SDL_FillRect(_surface, NULL, SDL_MapRGB(_surface->format, 0x00, 0x00, 0xFF));
  SDL_UpdateWindowSurface(_window);
  net_init();
  return;
}

void handle_input(level *level){
  SDL_Event event;
  bool key_up = true;
  Uint8 *kb_state = NULL;
  while(SDL_PollEvent(&event)){
    switch(event.type){
    case SDL_QUIT:
      _exit = true;
      break;
    }
  }
  kb_state = (Uint8*)SDL_GetKeyboardState(NULL);
  for(auto i = _local_player_list.begin(); i != _local_player_list.end(); i++){
    bool no_command = true;
    for(auto j = i->control_scheme.begin(); j!=i->control_scheme.end(); j++){
      if(kb_state[j->scancode]){
	if(std::find(_system_commands.begin(), _system_commands.end(), j->command)!=_system_commands.end()) 
	  handle_system_command(j->command);
	else
	  i->character->handle_command(j->command);
      }
    }
    if(no_command)
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

void handle_movement(level *current_level){
  for(auto i = current_level->actor_list.begin(); i!=current_level->actor_list.end(); i++){
    bool collision = false;
    if((*i)->collides==false)
      (*i)->move((*i)->position[0]+(*i)->velocity[0], (*i)->position[1] + (*i)->velocity[1]);
    else{
      for(auto j = current_level->actor_list.begin(); j!=current_level->actor_list.end(); j++){
	if(i==j)
	  continue;
      if((*j)->collides==true)
        if(handle_collision(*i, *j)){
	  collision=true;
	  break;
	}
      if(collision)
	break;
      }
      if(!collision)
	(*i)->move((*i)->position[0]+(*i)->velocity[0], (*i)->position[1] + (*i)->velocity[1]);
    }
  }
  return;
}

void draw_hud(){
  return;
}

void draw_screen(level *current_level){
  current_level->draw();
  for(auto i = current_level->actor_list.begin(); i != current_level->actor_list.end(); i++){
    if(*i)
      (*i)->draw();
  }
  draw_hud();
  SDL_UpdateWindowSurface(_window);
  return;
}

void logic_loop(){
  return;
}

void log_message(int level, std::string string){
  std::cout << level << string << std::endl;
  return;
}

void game_loop(level *level){
  unsigned int current, last, delay, tick;
  current=SDL_GetTicks();
  last=current;
  while(!_exit){
    std::list<actor*>::iterator i = level->actor_list.begin();
    if(tick % NET_RATE == 0){
      net_flush_messages();
      net_handle_messages();
    }
    handle_movement(level);
    draw_screen(level);
    delay=(1000/TICK_RATE - current + last);
    if(delay>0){
      SDL_Delay(delay);
    }
    else
      log_message(INFO, "%d Milliseconds of lag \n"); 
    tick++;
    handle_input(level);
    while(i!=level->actor_list.end()){
      auto prev=i;
      i++;
      (*prev)->update();
    }
    level->actor_list.remove_if([](actor *a){return a->remove;});
  }
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

void handle_system_command(std::string str){
  return;
}

#include "bomberbloke.h"
#include <SDL2/SDL.h>

int    _default_bomb_time = DEFAULT_BOMB_TIMER;
double _bloke_size[2]     = {DEFAULT_BLOKE_SIZE, DEFAULT_BLOKE_SIZE};
bool   _draw = true;

int main(){
  net_message msg;
  struct addrinfo hints, *res;
  socklen_t src_addr_len=sizeof(client_addr);
  net_join_server("", 0);
  return 0;
}



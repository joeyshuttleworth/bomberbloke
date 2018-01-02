#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
extern "C"{
#include "../engine/net.h"
}
#include "../engine/engine.h"

//int    _default_bomb_time = DEFAULT_BOMB_TIMER;
//double _bloke_size[2]     = {DEFAULT_BLOKE_SIZE, DEFAULT_BLOKE_SIZE};
const std::vector<command_binding> _default_bindings;
bool _draw = false;
bool _server = false;
unsigned int _state = 0;

int main (int argc, char **argv){
  pthread_t net_receive;
  //pthread_t read_console;
  net_messages_init();
  //pthread_create(&net_receive,  NULL, receive_loop, NULL);
  //pthread_create(&read_console, NULL, console_loop, NULL); 
  log_message(INFO, "Bomberbloke starting...\n");
  console_loop(NULL);
  return 0;
}

void handle_datagram(char *buf, struct sockaddr *client_addr, int count){
  std::cout << buf;
  return;
}

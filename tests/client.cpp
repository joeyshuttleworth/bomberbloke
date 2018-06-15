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
#include "../engine/level.h"

//int    _default_bomb_time = DEFAULT_BOMB_TIMER;
//double _bloke_size[2]     = {DEFAULT_BLOKE_SIZE, DEFAULT_BLOKE_SIZE};

const std::vector<command_binding> _default_bindings;
bool _draw = false;
bool _server = false;

int main (int argc, char **argv){
  pthread_t net_receive;
  pthread_t read_console;
  level *level1 = new level(10, 10);

  net_messages_init();
  pthread_create(&net_receive,  NULL, receive_loop, NULL);
  pthread_create(&read_console, NULL, console_loop, NULL); 
  log_message(INFO, "Bomberbloke starting...\n");
  init_engine(level1);
  client_loop(level1);
  return 0;
}

void handle_datagram(char *buf, struct sockaddr_storage *client_addr, unsigned int addr_len, unsigned int count){
  std::cout << "RECEIVED DATAGRAM: " << buf <<"\n";
  /*TODO: Check datagram is from server */
  if(buf[0] == NET_ACK){
    _state = PAUSED;
    std::cout << "Sucessfully joined server\n";
    return;
  }
}

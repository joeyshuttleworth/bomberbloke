#include "bomberbloke.h"
#include <SDL2/SDL.h>

int main (int argc, char **argv){
  pthread_t net_receive;
  pthread_t read_console;
  char *receive_port = (char*)malloc(sizeof(char)*5);
  receive_port = "8888";
  init_engine(NULL);
  log_message(INFO, "Bomberbloke dedicated server...\n");
  server_loop();
  return 0;
}


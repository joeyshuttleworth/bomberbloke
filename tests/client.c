#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "engine/net.h"
/*int    _default_bomb_time = DEFAULT_BOMB_TIMER;
double _bloke_size[2]     = {DEFAULT_BLOKE_SIZE, DEFAULT_BLOKE_SIZE};
bool _draw = false;
*/

int main (int argc, char **argv){
  struct addrinfo hints, *res;
  int sock;
  char buf[513] = "Hello Test test test\n";
  buf[0] = NET_JOIN; 
  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_protocol = 0;
  hints.ai_flags=AI_PASSIVE|AI_ADDRCONFIG;
  if(getaddrinfo(NULL, "8888", &hints, &res)!=0){
    printf("ERROR\n");
    return 1;
    /*Error*/
  }
  if((sock=socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1){
    printf("ERROR\n");
    return 1;
    /*Couldn't create socket*/
  }
  freeaddrinfo(res);
  sendto(sock, buf, sizeof(buf),0, res->ai_addr, res->ai_addrlen);
  return 0;
}

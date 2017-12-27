#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

/*int    _default_bomb_time = DEFAULT_BOMB_TIMER;
double _bloke_size[2]     = {DEFAULT_BLOKE_SIZE, DEFAULT_BLOKE_SIZE};
bool _draw = false;
*/
void handle_datagram(char *, int,struct sockaddr*);
int main (int argc, char **argv){
  struct sockaddr  client_addr;
  struct addrinfo hints, *res;
  socklen_t src_addr_len=sizeof(client_addr);
  int sock;
  char buf[513];
  //level *level1 = new level(10, 10);
  size_t count;
  //init_engine(level1);
  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_protocol = 0;
  hints.ai_flags=AI_PASSIVE|AI_ADDRCONFIG;
  if(getaddrinfo(NULL, "8888", &hints, &res)!=0){
    /*Error*/
  }
  if((sock=socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1){
    /*Couldn't create socket*/
  }
  
  if(bind(sock, res->ai_addr, res->ai_addrlen) == -1){
    /*Couldn't bind socket*/
  }
  freeaddrinfo(res);
  while(1){
    count = recvfrom(sock, buf, sizeof(buf),0,(struct sockaddr*)&client_addr, &src_addr_len);
    handle_datagram(buf, count, &client_addr);
  }
  return 0;
}

void handle_datagram(char *buffer, int count, struct sockaddr *client_addr){
  buffer[513]='\0';
  printf("%s \n", buffer);
  return;
}

#include "engine.h"
#include "player.h"
extern "C"{
#include "net.h"
}
#include "server.h"

std::list<network_p> _client_list;
bool _draw = false;
const std::vector<command_binding> _default_bindings;

bool is_client(sockaddr *addr){
  for(network_p client : _client_list){
    bool equal = true;
    if(addr->sa_family != client.address.sa_family)
      equal = false;
    else{
      for(int i = 0; i<14; i++){
	if(addr->sa_data[i]!=client.address.sa_data[i]){
	  equal = false;
	  break;
	}
      }
    }
    if(equal)
      return true;
  }
  return false;
}

int main (int argc, char **argv){
  pthread_t net_receive;
  net_messages_init();
  pthread_create(&net_receive, NULL, receive_loop, NULL);
  log_message(INFO, "Bomberbloke dedicated server...\n");
  server_loop();
  return 0;
}

void server_loop(){
  while(true){
    sleep(1);
  }
  return;
}

void server_handle_datagram(char *buf, struct sockaddr *client_addr, int count){
  char opcode = buf[0];
  char *data  = buf + 1;

  if(opcode == NET_JOIN){
    char *nickname = buf + 1;
    nickname[count-1] = '\0';
    printf("JOIN REQUEST RECEIVED FROM %s\n", nickname);
    if(is_client(client_addr))
      std::cout << nickname << " is already joined" << "\n";
    else{
      _client_list.push_back(network_p(std::string(nickname), client_addr));
      std::cout << nickname << " has joined the server" << "\n";
    }
  }
  return;
}

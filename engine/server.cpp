#include "server.h"

std::list<network_p> _client_list;
bool _draw = true;
bool _server = true;
unsigned int _state = 0;
const std::vector<command_binding> _default_bindings;

std::list<network_p>::iterator get_client(sockaddr *addr){
  for(auto client = _client_list.begin(); client!=_client_list.end(); client++){
    bool equal = true;
    if(addr->sa_family != client->address.sa_family)
      equal = false;
    else{
      for(int i = 0; i<14; i++){
	if(addr->sa_data[i]!=client->address.sa_data[i]){
	  equal = false;
	  break;
	}
      }
    }
    if(equal)
      return client;
  }
  return _client_list.end();
}

int main (int argc, char **argv){
  pthread_t net_receive;
  pthread_t read_console;
  net_messages_init();
  pthread_create(&net_receive,  NULL, receive_loop, NULL);
  pthread_create(&read_console, NULL, console_loop, NULL); 
  log_message(INFO, "Bomberbloke dedicated server...\n");
  server_loop();
  return 0;
}

void server_loop(){
  while(true){
    if(_client_list.size()==0)
      //state = STOPPED;
    
    sleep(1);
  }
  return;
}

void handle_datagram(char *buf, struct sockaddr *client_addr, int count){
  char opcode = buf[0];
  char *data  = buf + 1;

  switch(opcode){
  case NET_JOIN:{
    char nickname[count];
    memcpy(&nickname, buf + 1, count - 1);
    buf[count] = '\0';
    nickname[count-1] = '\0';
    printf("JOIN REQUEST RECEIVED FROM %s\n", nickname);
    if(get_client(client_addr)!=_client_list.end()){
      net_message msg;
      msg.operation = NET_ACK;
      msg.data   = "Thank you for playing bomberbloke.\n";
      std::cout << "The nickname \"" << nickname << "\" is already taken\n";
    }
    else{
      _client_list.push_back(network_p(std::string(nickname), client_addr));
      std::cout << nickname << " has joined the server" << "\n";
    }
    break;
  }
  case NET_LEAVE:{
    auto client = get_client(client_addr);
    if(client!=_client_list.end()){
      std::cout << client->nickname << " has left the server\n";
      _client_list.erase(client);
    }
    else{
      log_message(DEBUG, "Leave request received from non-connected player\n");
    }
    break;
  }
  case NET_MOVE:{
    
    break;
  }
  default:
    break;
  }
  return;
}

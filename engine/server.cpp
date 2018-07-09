#include "server.h"
unsigned int _tick;
std::list<network_p> _client_list;
bool _draw = true;
bool _server = true;
unsigned int _ping_time = 0;
const std::vector<command_binding> _default_bindings;

std::list<network_p>::iterator get_client(sockaddr_storage *addr){
  for(auto client = _client_list.begin(); client!=_client_list.end(); client++){
    int family = addr->ss_family;
    if(family == client->address->ss_family){
      if(family == AF_INET){
	if(((sockaddr_in*)addr)->sin_addr.s_addr == ((sockaddr_in*)client->address)->sin_addr.s_addr && ((sockaddr_in*)addr)->sin_port == ((sockaddr_in*)client->address)->sin_port)
	  return client;
	
      }
      else if(family == AF_INET6){
	//TODO IPV6
      }
    }
  }
return _client_list.end();
}

void server_init(){
  net_messages_init();
  return;
}

void remove_player(std::list<network_p>::iterator player){
  _client_list.erase(player);
  net_remove_messages_to(player->address);
  return;
}

int main (int argc, char **argv){
  pthread_t net_receive;
  pthread_t read_console;
  char *receive_port = (char*)malloc(sizeof(char)*5);
  receive_port = "8888";
  server_init();
  pthread_create(&net_receive,  NULL, receive_loop, (void*) receive_port);
  pthread_create(&read_console, NULL, console_loop, NULL); 
  log_message(INFO, "Bomberbloke dedicated server...\n");
  server_loop();
  return 0;
}

void server_loop(){
  while(true){
    if(_client_list.size()==0){
      _state = STOPPED;
    }
    net_flush_messages();
    _ping_time = _tick; 
    for(auto i = _client_list.begin(); i != _client_list.end(); i++){
      if(_tick - i->last_ping > 30*TICK_RATE)
	remove_player(i);
      else if(_tick < i->last_ping)
	i->last_ping = 0;
      i->ping();
    }
    sleep(1);
  }
  return;
}

void handle_datagram(char *buf, struct sockaddr_storage *client_addr, unsigned int count, unsigned int addr_len){
  char opcode = buf[0];
  char *data  = buf + 1;
  switch(opcode){
  case NET_JOIN:{
    char nickname[count];
    memcpy(&nickname, buf + 2, count - 1);
    buf[count] = '\0';
    nickname[count-1] = '\0';
    printf("JOIN REQUEST RECEIVED FROM %s\n", nickname);
    if(get_client(client_addr)!=_client_list.end()){
      net_message msg;
      msg.operation = NET_ACK;
      msg.data = (char*)malloc(sizeof(char));
      msg.data[0]= data[0];
      msg.data_size = strlen(msg.data);
      msg.address_length = addr_len;
      msg.frequency = 0;
      msg.attempts  = 0;
      memcpy(&msg.address,client_addr, sizeof(addr_len)); 
      net_add_message(&msg);
      log_message(DEBUG, "client is already joined\n");
    }
    else{
      net_message msg;
      msg.operation = NET_ACK;
      msg.data = (char *)malloc(sizeof(char));
      msg.data[0]  = data[0];
      msg.data_size = 1;
      msg.address_length = addr_len;
      msg.frequency = 0;
      msg.attempts  = 0;
      memcpy(&msg.address,client_addr, sizeof(addr_len)); 
      net_add_message(&msg);
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
  case NET_ACK:{
    // switch(){
    //}
    unsigned int message_id = ntohs(buf[1]);
    net_message_node *current = net_get_message(message_id);
    net_remove_message(current);
  }
  default:
    break;
  }
  return;
}

void timeout(net_message_node *node){
  net_message *msg = node->msg;
  switch(msg->operation){
  default:
    break;
  }
  return;
}

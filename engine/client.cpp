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
  char *receive_port = (char*)malloc(sizeof(char) * 5);
  receive_port = "8889";
  net_messages_init();
  pthread_create(&net_receive,  NULL, receive_loop, (void*)receive_port);
  pthread_create(&read_console, NULL, console_loop, NULL); 
  log_message(INFO, "Bomberbloke starting...\n");
  init_engine(level1);
  client_loop(level1);
  return 0;
}

void handle_datagram(char *buf, struct sockaddr_storage *client_addr, unsigned int addr_len, unsigned int count){
  std::cout << "RECEIVED DATAGRAM: " << buf[0] << buf+1 <<"\n";
  /*TODO: Check datagram is from server */
  
  switch(buf[0]){
  case NET_ACK:{      
    unsigned int message_id = ntohs(buf[1]);
    net_message_node *current = net_get_message(message_id);
    net_message *msg = NULL;
    
    if(!current)
      log_message(DEBUG, "Empty datagram");
    else{
      msg = current->msg;
      switch(msg->operation){
      case NET_JOIN:
	std::cout << "Sucessfully joined server\n";
	_state = PAUSED;
	pthread_mutex_lock(&net_out_mutex);
	net_remove_message(current);
	pthread_mutex_unlock(&net_out_mutex);
	break;
      default:
	pthread_mutex_lock(&net_out_mutex);
	net_remove_message(current);
	pthread_mutex_unlock(&net_out_mutex);
	break;
      }
      break;
    }
  }
  case NET_PING:{
    if(count != 2){
      log_message(ERROR, "Received malformed ping");
    }
    net_message msg;
    msg.operation = NET_ACK;
    msg.data = (char*)malloc(sizeof(char));
    msg.data[0]= buf[1];
    msg.data_size = 1;
    msg.address_length = addr_len;
    msg.frequency = 0;
    msg.attempts  = 0;
    memcpy(&msg.address,client_addr, sizeof(addr_len)); 
    net_add_message(&msg);
    log_message(DEBUG, "PING received from server");
  }
  }
  return;
}

void timeout(net_message_node *node){
  unsigned int opcode = node->msg->operation;
  //  std::cout << "opcode" << opcode << " timed out\n";
  switch(opcode){
  case NET_JOIN:
    _state = DISCONNECTED;
    break;
  }
  return;
}

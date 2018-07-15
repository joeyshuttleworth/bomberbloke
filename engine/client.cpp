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
unsigned int _tick =0;
unsigned int _last_receive = 0;

int main (int argc, char **argv){
  pthread_t net_receive;
  pthread_t read_console;
  level *level1 = new level(10, 10);
  char *receive_port = (char*)malloc(sizeof(char) * 5);
  receive_port = "8889";
  net_messages_init();
  pthread_create(&net_receive,  NULL, receive_loop, NULL);
  pthread_create(&read_console, NULL, console_loop, NULL); 
  log_message(INFO, "Bomberbloke starting...\n");
  init_engine(level1);
  client_loop(level1);
  return 0;
}

void handle_datagram(char *buf, struct sockaddr_storage *client_addr, unsigned int addr_len, unsigned int count){
  std::cout << "RECEIVED DATAGRAM: " << buf[0] << buf+1 <<"\n";
  /*TODO: Check datagram is from server */
  _last_receive = _tick;
  if(_state==DISCONNECTED)
    return;  
  switch(buf[0]){
  case NET_ACK:{      
    unsigned int message_id = buf[1];
    net_message_node *current = net_get_message(message_id);
    net_message *msg = NULL;
    
    if(!current)
      log_message(DEBUG, "Malformed NET_ACK datagram");
    else{
      msg = current->msg;
      switch(msg->operation){
      case NET_JOIN:
	if(_state == JOINING){
	std::cout << "Sucessfully joined server\n";
	_state = PAUSED;
	}
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
    }
    break;
  }
  case NET_PING:{
    if(count != 2){
      log_message(ERROR, "Received malformed ping");
    }
    else{
      net_message msg;
      msg.operation = NET_ACK;
      msg.data = (char*)malloc(sizeof(char));
      msg.data[0]= buf[1];
      msg.data_size = 1;
      msg.address_length = sizeof(struct sockaddr_storage);
      msg.frequency = 0;
      msg.attempts  = 1;
      memcpy(&msg.address, client_addr, addr_len); 
      net_add_message(&msg);
      log_message(DEBUG, "PING received from server");
    }
    break;
    }
  case NET_MSG:{
    char str[count-1];
    strncpy(str, buf+2,count-1);
    str[count-2] = '\0';
    puts(str);
    break;
  }
  }
  return;
}


void client_loop(level *level){
  unsigned int current=0, last;
  int delay;

  while(!_halt){
    switch(_state){
    case DISCONNECTED:
      sleep(1);
      break;
    case JOINING:
      _last_receive = _tick;
    default:{
      if(_tick - _last_receive > 30*TICK_RATE){
	handle_system_command(split_to_tokens("disconnect"));
	log_message(INFO, "Disconnecting - timed out");
      }
      last = current;
      // std::list<actor*>::iterator i = level->actor_list.begin();
      if(_tick % NET_RATE == 0){
	net_flush_messages();
      }
      //handle_movement(level);
      //if(_draw)
	//	draw_screen(level);
      delay=(1000/TICK_RATE) - current + last;
      if(delay>0){
	SDL_Delay(delay);
      }
      _tick++;
      //handle_input(level);
      /*while(i!=level->actor_list.end()){
	auto prev=i;
	i++;
	(*prev)->update();
      */
      //}
      //level->actor_list.remove_if([](actor *a){return a->remove;});
    }
      break;
    }
  }
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

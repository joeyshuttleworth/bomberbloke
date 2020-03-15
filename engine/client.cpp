#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "engine.h"

unsigned int _last_receive;
bool _draw = false;
bool _server = false;

void handle_datagram(char *buf, struct sockaddr_storage *client_addr, unsigned int addr_len, unsigned int count){
  std::cout << "RECEIVED DATAGRAM: " << buf[0] << buf[1] << buf + 2 <<"\n";
  /*TODO: Check datagram is from server */
  _last_receive = _tick;
  if(_state==DISCONNECTED)
    return;  

  // Flag to track whether we need to acknowledge the packet or not
  bool send_ack = false;
  switch(buf[0]){
  case NET_ACK:{      
    unsigned int message_id;
    list_node *current;
    net_message *msg = NULL;
    
    if(count < 3){
      log_message(DEBUG, "Received malformed ACK - too short");
      return;
    }
    
    message_id = buf[1] + (buf[2] << 8);
    current = net_get_message(message_id);
    if(!current)
      log_message(DEBUG, "Malformed NET_ACK datagram - no corresponding message");
    else{
      msg = (net_message *)current->data;
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
    if(count != 3){
      log_message(ERROR, "Received malformed ping");
    }
    else{
      send_ack = true;
      log_message(DEBUG, "PING received from server");
    }
    break;
    }
  case NET_MSG:{
    char str[count-3];
    strncpy(str, buf+3,count-2);
    str[count-3] = '\0';
    std::cout << str << "\n";
    break;
  }
  case NET_START:{
    send_ack = true;
    break;
  }
  case NET_NEW_GAME:{
    send_ack = true;

    /*TODO: Create new actor_list */
    std::list<actor> new_actor_list; 
    log_message(INFO, "Starting new game...\n");
    _state = STOPPED;
    engine_new_game("");
    break;
  }
  }
  
  if(send_ack){
    //Send a packet acknowledging that we received the packet
    net_message msg;
    msg.operation = NET_ACK;
    msg.data = (char*)malloc(sizeof(char));
    msg.data[0]= buf[1];
    msg.data[1] = buf[2];
    msg.data_size = 1;
    msg.address_length = sizeof(struct sockaddr_storage);
    msg.frequency = 0;
    msg.attempts  = 1;
    memcpy(&msg.address, client_addr, addr_len); 
    net_add_message(&msg, false);

  }
  return;
}


void client_loop(){
  unsigned int current=0, last;
  int delay;

  while(!_halt){
     last = current;
     if(_tick % NET_RATE == 0){
       net_flush_messages();
     }
     delay=(1000/TICK_RATE) - current + last;
     if(delay>0){
	SDL_Delay(delay);
      }
    switch(_state){
    case DISCONNECTED:
      sleep(1);
      break;
    case JOINING:
      _last_receive = _tick;
      break;
    default:{
      if(_tick - _last_receive > 30*TICK_RATE){
	handle_system_command(split_to_tokens("disconnect"));
	log_message(INFO, "Disconnecting - timed out");
      }
      handle_input(&_level);
      if(_state == PLAYING){
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
    draw_screen();
    _tick++;
  }
}


void timeout(list_node *node){
  net_message *msg = (net_message *) node->data;
  unsigned int opcode = msg->operation;
  //  std::cout << "opcode" << opcode << " timed out\n";
  switch(opcode){
  case NET_JOIN:
    _state = DISCONNECTED;
    break;
  }
  return;
}


void engine_new_game(std::string tokens){
  log_message(INFO, (char*)"New game ready to start...\n");
  _state = PAUSED;
  new_game(tokens);
  _level = level(10,10);
  //remove moves and SYNCS from message queue
  return;
}

void engine_start_game(){
  return;
}

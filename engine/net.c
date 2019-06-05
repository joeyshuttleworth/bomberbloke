#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "net.h"
#include "err.h"
#include "state.h"

pthread_mutex_t net_out_mutex;
uint8_t _current_id = 0;
list_node *_net_out_head = NULL, *_net_out_current = NULL, *_net_in_head = NULL, *_net_in_current;
list_node *_net_multi_in_head, *_net_multi_out_head;
bool _net_on = true;
int _server_socket = -1;
unsigned int _net_tick;
FILE *f_out, *f_in; 
struct sockaddr_storage _server_address;

void net_exit(){
  if(f_out)
    fclose(f_out);
  if(f_in)
    fclose(f_in);
  net_clear_messages();
  return;
}

void net_messages_init(){
  if(IS_SERVER){
    f_out = fopen("s_net_out.bin", "wb");
    f_in  = fopen("s_net_in.bin", "wb");
  }
  else{
    f_out = fopen("net_out.bin", "wb");
    f_in  = fopen("net_in.bin",  "wb");
  }
  _net_out_head = NULL;
  _net_in_head  = NULL;
  _net_multi_in_head = NULL;
  _net_multi_out_head = NULL;
  memset(&_server_address, 0, sizeof(_server_address));
  _net_tick = 0;
  return;
}

uint8_t get_unused_out_id(){
  uint8_t id = 0;
  for(int i=0;i!=255;i++){
    int used = 0;
    //Check id isn't in use in the _net_out list
    if(_net_out_head){
      list_node *current = _net_out_head;
      while(current){
	net_message *msg = current->data;
	if(msg->id == id)
	  used = 1;	
	if(!current->next)
	  break;
	else
	  current = current->next;	
      }
    }
    if(_net_multi_out_head && !used){
      list_node *current = _net_multi_out_head;
      while(current){
	net_message *msg = current->data;
	if(msg->id == id)
	  used = 1;	
	if(!current->next)
	  break;
	else
	  current = current->next;	
      }
    }
  }
  return id;
}

void net_add_message(net_message *msg, bool locked){
  list_node *current;
  bool reserved = false;
  if(!locked)
    pthread_mutex_lock(&net_out_mutex);
  msg->id = _current_id;
  //TODO make sure current_id isn't reserved
  _current_id++;
  current = _net_out_head;
  if(!_net_out_current){
    _net_out_head = malloc(sizeof(net_message));
    _net_out_current = _net_out_head;
    _net_out_head->next = NULL;
  }
  else{
    _net_out_current->next = malloc(sizeof(net_message));
    _net_out_current = _net_out_current->next;
    _net_out_current->next = NULL;
  }
  _net_out_current->data = (net_message *)malloc(sizeof(net_message));
  memcpy(_net_out_current->data, msg, sizeof(net_message));
  ((net_message *) _net_out_current->data)->data = malloc(msg->data_size);
  memcpy(((net_message *) _net_out_current->data)->data, msg->data, msg->data_size);
  msg->offset = (uint8_t)_net_tick;
  if(!locked)
    pthread_mutex_unlock(&net_out_mutex);
  if(msg->attempts ==0)
    printf("WARNING: attempts = 0 - messages will be deleted without sending\n");
  return;
}

void net_flush_messages(){
  struct sockaddr_storage *address = &_server_address;
  if(!_net_out_current)
    return;
  else{
    printf("tick: %d\n", _net_tick);
    list_node *current = _net_out_head;
    list_node *prev = NULL;
    pthread_mutex_lock(&net_out_mutex);
    while(current){
      net_message *msg = current->data;
      if(msg->frequency!=0){
	if((int)(_net_tick - msg->offset) % msg->frequency != 0){
	  prev    = current;
	  current = current->next;
	  continue;
	}
      }
      if(msg->attempts==0){
	list_node *remove = current;
	if(prev){
	  if(current->next)
	    prev->next = current->next;
	  else
	    prev->next = NULL;
	}
	timeout(current);
	if(current->next)
	  current=current->next;
	else current = NULL;
	net_remove_message(remove);
	continue;		  
      }
      net_send_message(msg);
      msg->attempts--;
      prev = current;
      if(current->next)
	current=current->next;
      else break;
    }
    _net_out_current = _net_out_head;
    if(_net_out_head){
      while(_net_out_current->next)
	_net_out_current = _net_out_current->next;
    }
    pthread_mutex_unlock(&net_out_mutex);
  }
  _net_tick++;
  return;
}

void net_send_message(net_message *msg){
  struct sockaddr_storage *address = &(msg->address);
  unsigned int datagram_size = 0;
  char datagram[3+msg->data_size];
  if(!IS_SERVER)
    address = &_server_address;
  switch(msg->operation){
  default:
	break;
  case NET_MOVE:
    /* memcpy(&id, msg->data, sizeof(uint32_t));
    memcpy(coords, msg->data + sizeof(uint32_t), 4*sizeof(double));
    printf("SEND - MOVE %u: %lf, %lf\n %lf, %lf\n", id, coords[0], coords[1], coords[2], coords[3]); 
    */break;
  case NET_PING:{
    datagram[0] = msg->operation;
    datagram[1] = msg->id;
    datagram[2] = msg->id >> 8;
    datagram_size = 3;
    printf("Sending Ping\n");
    break;
  }
  case NET_LEAVE:
    datagram[0] = msg->operation;
    datagram_size = 1;
    break;
  case NET_ACK:{
    datagram[0] = msg->operation;
    memcpy(datagram + 1, msg->data, 2);
    datagram_size = 3;
    printf("Sending ACK\n");
    break;}
  case NET_NEW_GAME: case NET_START: case NET_MSG: case NET_JOIN:{
    datagram[0] = msg->operation;
    datagram[1] = msg->id;
    datagram[2] = msg->id >> 8;
    memcpy(datagram+3, msg->data, msg->data_size);
    datagram_size = msg->data_size + 3;
    break;
  }
  case NET_ACTOR_LIST: case NET_SYN:{
    //send the multi package and store the id in the _out list
    void *data = malloc(sizeof(msg->id));
    net_message multi_message;
    *(uint16_t*)data = msg->id;
    unsigned int no_packets = (msg->data_size / 500) + 1;
    if(no_packets > 255){
      printf("\nError: message too large to send \n");
      return;
    }
    list_add(&_net_multi_out_head, data);
    multi_message.attempts = DEFAULT_ATTEMPTS;
    multi_message.frequency = 1;
    multi_message.operation = NET_MULTI;
    //break the data into packets with maximum size 500 bytes
    multi_message.data = malloc(sizeof(char) * 500);
    for(unsigned int i = 0; i <= no_packets; i++){
      memset(multi_message.data, 0, 500);
      unsigned int length = 503;
      multi_message.data[0] = msg->id;
      multi_message.data[1] = msg->id >> 8;
      multi_message.data[2] = i;
      if(i == no_packets)
	length = (no_packets-1) * 500 - msg->data_size;
      memcpy(multi_message.data, data + 500*i, length+3);
      net_add_message(&multi_message, true);
    }
    break;
  }
  }
  if(datagram_size>0){
    fwrite(datagram, datagram_size, 1, f_out);
    if(address->ss_family == AF_INET)
      sendto(_server_socket, datagram, datagram_size, 0, (struct sockaddr*)address, sizeof(struct sockaddr_in));
    else if(address->ss_family == AF_INET6)
      sendto(_server_socket, datagram, datagram_size, 0, (struct sockaddr*)address, sizeof(struct sockaddr_in6));
    else{
      printf("error: malformed address\n");
    }
  }
  return;
}

void net_clear_messages(){
  list_node *current=_net_out_head;
  pthread_mutex_lock(&net_out_mutex);
  while(current){
    list_node *tmp = current;
    if(current->data){
      net_message *message = current->data;
      if(message->data){
	free(message->data);
      }
      free(current->data);
    }
    current=current->next;
    free(tmp);
  }
  _net_out_head = NULL;
  _net_out_current = NULL;
  pthread_mutex_unlock(&net_out_mutex);
  return;
}

void *receive_loop(void *a){
  struct sockaddr  client_addr;
  socklen_t src_addr_len=sizeof(client_addr);
  int sock;
  char buf[513];
  int optval = 1;
  size_t count;
  struct sockaddr_storage addr;
  int addr_len;

  if(a){
    struct addrinfo hints, *res;
    net_messages_init();
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = 0;
    hints.ai_family = AF_INET;
    hints.ai_flags=AI_PASSIVE|AI_ADDRCONFIG;
    if(getaddrinfo(NULL, (char *)a, &hints, &res)!=0){
      perror("Couldn't get address info\n");
      exit(1);
      /*Error*/
    }
    if((_server_socket=socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1){
      printf("Couldn't open socket\n");
      /*Error*/
    }
    addr_len = res->ai_addrlen;
    memcpy(&addr, res->ai_addr, addr_len);
    freeaddrinfo(res);
  }
  else{
    _server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr_len = sizeof(struct sockaddr_in);
  }
  if(setsockopt(_server_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int))==-1){
      perror("setsockopt");
      exit(1);
  }
  if(bind(_server_socket, (struct sockaddr*)&addr, addr_len) == -1){
    printf("ERROR Coudln't bind socket\n");
    perror("Bind failed");
    exit(1);
  } 
   while(1){
    memset(buf, 0, sizeof(buf));
    count = recvfrom(_server_socket, buf, 513,0, &client_addr, &src_addr_len);
    if(count>512){
      count = 512;
    }
    fwrite(buf, count, 1, f_in);
    handle_datagram(buf, &client_addr, src_addr_len, count);
   }
    return 0;
}
    
void net_join_server(const char *address, const char *port, const char *nickname){
  char *buf;
  int count = strlen(nickname) + 1;
  
  _state = JOINING;
  struct addrinfo hints;
  struct addrinfo *res;
  net_message msg;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family   = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_protocol = 0;
  hints.ai_flags=AI_PASSIVE|AI_ADDRCONFIG;
  if(getaddrinfo(address, port, &hints, &res)!=0){
    printf("Invalid address\n");
  }
  memcpy(&_server_address, res->ai_addr, res->ai_addrlen);
  freeaddrinfo(res);
  buf = (char *)malloc(count*sizeof(char));
  msg.operation = NET_JOIN;
  msg.data_size = strlen(nickname)+1;
  msg.frequency = 0;
  msg.attempts = 10;
  msg.data = nickname;
  net_add_message(&msg, false);
  printf("Joining server...\n");
  return;
}

void net_remove_message(list_node *node){
  if(!node)
    return; /*Error: null message*/
  else if(node == _net_out_head){
    if(_net_out_head->next)
      _net_out_head = _net_out_head->next;
    else{
      _net_out_head = NULL;
      _net_out_current = NULL;
    }
    if(node->data){
      net_message *msg = node->data;
      if(msg->data && msg->data_size>0)
	free(msg->data);
      free(msg);
    }
    free(node);
  }
  else{
    list_node *current = _net_out_head;
    
    while(current){
      if(current->next == node){
	current->next = node->next;
	if(node->data){
	  net_message *msg = node->data;
	  if(msg->data && msg->data_size>0)
	    free(msg->data);
	  free(node->data);
	}
	free(node);
	break;
      }
      current = current->next;
    }
  }
  /*Error: no message removed*/
  return;
}

list_node *net_get_message(unsigned int id){
  if(_net_out_head){
    list_node *current = _net_out_head;
    bool lower = ((net_message *)_net_out_head->data)->id > id;
    while(current){
      net_message *msg = current->data;
      if(msg->id == id)
	return current;
      if(lower && msg->id>id)
	return NULL;
      current=current->next;
    }
  }
  return NULL;;
}

void net_remove_messages_to(struct sockaddr_storage *address){
  if(_net_out_head){
    list_node *current = _net_out_head;

    if(address->ss_family == AF_INET){
      struct sockaddr_in *remove_address = (struct sockaddr_in*) address;
      while(1){
	net_message *msg = current->data;
	if(msg->address.ss_family != AF_INET)
	  break;
	if(((struct sockaddr_in*)address)->sin_addr.s_addr == ((struct sockaddr_in*)&msg->address)->sin_addr.s_addr && ((struct sockaddr_in*)address)->sin_port == ((struct sockaddr_in*)&msg->address)->sin_port)
	  if(current->next)
	    current=current->next;
	  else
	    break;
      }
    }
  /*else if(address->ss_family == AF_INET6){
    struct sockaddr_in6 *remove_address = address;
      while(1){
      if(((struct sockaddr*)current->msg->address).ss_port==remove_address.ss_port){
	if(((struct sockaddr*) current->->msg->address).ss_address == remove_address.ss_address)
	  remove_message(current);
      }
      if(current->next)
	  current=current->next;
	else
	  break;
      }
      }*/
  }
  return;
}

void net_float_create(double val, net_float *res){
  int exponent;
  res->sign  = val>0;
  res->mantissa = frexp(val, &exponent);
  res->exponent = exponent;
  return;
}

void list_add(list_node **head, void *data){
  if(*head == NULL){
    *head = malloc(sizeof(list_node));
    (*head)->data = data;
    return;
  }
  else{
    list_node *current = *head;
    while(current->next){
      current = current->next;
    }
    current->next = malloc(sizeof(list_node));
    current = current->next;
    current->next = NULL;
    current->data = data;
    return;
  }
}

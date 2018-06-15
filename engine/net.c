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
net_message_node *_net_out_head = NULL, *_net_out_current = NULL, *_net_in_head = NULL, *_net_in_current;
bool _net_on = true;
int _server_socket = -1;
unsigned int _net_tick;
void clear_messages();
struct sockaddr_storage _server_address;

void net_messages_init(){
  _net_out_head = NULL;
  _net_in_head  = NULL;
  memset(&_server_address, 0, sizeof(_server_address));
  _net_tick = 0;
  return;
}

void net_add_message(net_message *msg){
  pthread_mutex_lock(&net_out_mutex);
  msg->id = _current_id;
  _current_id++;
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
  _net_out_current->msg = malloc(sizeof(net_message));
  _net_out_current->msg->data = malloc(msg->data_size);
  memcpy(_net_out_current->msg->data, msg->data, msg->data_size);
  memcpy(_net_out_current->msg, msg, sizeof(net_message));
  pthread_mutex_unlock(&net_out_mutex);
  return;
}

void net_flush_messages(){
  if(!_net_out_current)
    return;
  else{
    net_message_node *current = _net_out_head;
    net_message_node *prev = NULL;
    pthread_mutex_lock(&net_out_mutex);
    while(current){
      net_message *msg = current->msg;
      double coords[4];
      uint32_t id;
      char datagram[2 + msg->data_size];
      if(msg->data_size == 0){
	if(prev)
	  prev->next = current->next;
	else
	  _net_out_head = current->next;
	free(current->msg);
	free(current);
	continue;
      }
      if(_net_tick % msg->repeat != 0)
	continue;
      switch(msg->operation){
      case NET_MOVE:
	memcpy(&id, msg->data, sizeof(uint32_t));
	memcpy(coords, msg->data + sizeof(uint32_t), 4*sizeof(double));
	printf("SEND - MOVE %u: %lf, %lf\n %lf, %lf\n", id, coords[0], coords[1], coords[2], coords[3]); 
	break;
      default:case NET_ACK:
	datagram[0] = msg->operation;
	memcpy(datagram + 1, msg->data, msg->data_size);
	if(msg->address.ss_family == AF_INET)
	  sendto(_server_socket, datagram, msg->data_size + 1, 0, (struct sockaddr*)&(msg->address), sizeof(struct sockaddr_in));
	else if(msg->address.ss_family == AF_INET6)
	  sendto(_server_socket, datagram, msg->data_size + 1, 0, (struct sockaddr*)&(msg->address), sizeof(struct sockaddr_in6));  
	break;
      case NET_JOIN:
	datagram[0] = msg->operation;
	datagram[1] = msg->id;
	memcpy(datagram + 2, msg->data, msg->data_size);
	if(msg->address.ss_family == AF_INET)
	  sendto(_server_socket, datagram, msg->data_size + 2, 0, (struct sockaddr*)&(msg->address), sizeof(struct sockaddr_in));
	else if(msg->address.ss_family == AF_INET6)
	  sendto(_server_socket, datagram, msg->data_size + 2, 0, (struct sockaddr*)&(msg->address), sizeof(struct sockaddr_in6));  
	break;
      }
      if(!msg->attempts){
	if(prev)
	  prev->next = current->next;
	else
	  _net_out_head = current->next;
	free(current->msg);
	free(current);
      }
      else
	msg->attempts--;
      prev    = current;
      current = current->next;
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

void clear_messages(){
  net_message_node *current=_net_out_head->next;
  memset(_net_out_head, 0, sizeof(net_message_node));
  while(current){
    net_message_node *tmp = current;
    current=current->next;
    free(tmp);
  }
  _net_out_current = _net_out_head;
  return;
}

void *receive_loop(void *a){
  struct sockaddr_storage  client_addr;
  struct addrinfo hints, *res;
  socklen_t src_addr_len=sizeof(client_addr);
  int sock;
  char buf[513];
  int optval = 1;
  //level *level1 = new level(10, 10);
  size_t count;
  //init_engine(level1);
  net_messages_init();
  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_protocol = 0;
  hints.ai_flags=AI_PASSIVE|AI_ADDRCONFIG;
  if(getaddrinfo(NULL, "8888", &hints, &res)!=0){
    perror("Coudln't get address info\n");
    exit(1);
      /*Error*/
  }
  if((_server_socket=socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1){
    printf("Couldn't open socket\n");
    /*Error*/
  }
  if(setsockopt(_server_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int))==-1){
      perror("setsockopt");
      exit(1);
  }
  if(bind(_server_socket, res->ai_addr, res->ai_addrlen) == -1){
    printf("ERROR Coudln't bind socket\n");
    perror("Bind failed");
    exit(1);
    /*Couldn't bind socket*/
  }
  freeaddrinfo(res);
  while(1){
    memset(buf, 0, sizeof(buf));
    count = recvfrom(_server_socket, buf, 513,0, &client_addr, &src_addr_len);
    if(count>512){
      count = 512;
    }
    handle_datagram(buf, &client_addr, src_addr_len, count); 
  }
    return 0;
}
    
void net_join_server(const char *address, int port, const char *nickname){
  char *buf;
  int count = strlen(nickname) + 1;
  _state = JOINING;
  struct addrinfo hints;
  struct addrinfo *res;
  net_message msg;
  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_protocol = 0;
  hints.ai_flags=AI_PASSIVE|AI_ADDRCONFIG;
  if(getaddrinfo(NULL, "8889", &hints, &res)!=0){
    printf("Invalid address\n");
  }
  buf = (char *)malloc(count*sizeof(char));
  msg.operation = NET_JOIN;
  msg.data_size = strlen(nickname+1);
  msg.repeat = NET_RATE*2;
  msg.attempts = 10;
  msg.data = nickname;
  net_add_message(&msg);
  printf("Joining server\n");
  memcpy(&_server_address, res->ai_addr, sizeof(_server_address));
  freeaddrinfo(res);
  return;
}

#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include "net.h"
#include "err.h"

net_message_node *_net_out_head = NULL, *_net_out_current = NULL, *_net_in_head = NULL, *_net_in_current;
bool _net_on = true;
struct sockaddr_in server_address;
int server_socket = -1;
void clear_messages();

void net_messages_init(){
  _net_out_head = malloc(sizeof(net_message_node));
  _net_in_head  = malloc(sizeof(net_message_node));
  _net_out_current = _net_out_head;
  _net_in_current  = _net_in_head;
  _net_out_head->next = NULL;
  _net_in_head->next  = NULL;
  memset(&server_address, 0, sizeof(server_address));
  return;
}

void net_add_message(net_message *msg){
  _net_out_current->msg = malloc(sizeof(net_message));
  memcpy(_net_out_current->msg, msg, sizeof(net_message));
  _net_out_current->next = malloc(sizeof(net_message_node));
  _net_out_current = _net_out_current->next;
  _net_out_current->next = NULL;
}

void net_flush_messages(){
  net_message_node *current = _net_out_head;
  while(current){
    net_message *msg = current->msg;
    double coords[4];
    uint32_t id;
    switch(msg->operation){
    case NET_MOVE:
      memcpy(&id, msg->data, sizeof(uint32_t));
      memcpy(coords, msg->data + sizeof(uint32_t), 4*sizeof(double));
      printf("SEND - MOVE %u: %lf, %lf\n %lf, %lf\n", id, coords[0], coords[1], coords[2], coords[3]); 
      break;
    default:
      break;
      
    }
    current=current->next;
  }
  clear_messages();
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
  struct sockaddr  client_addr;
  struct addrinfo hints, *res;
  socklen_t src_addr_len=sizeof(client_addr);
  int sock;
  char buf[513];
  int optval = 1;
  //level *level1 = new level(10, 10);
  size_t count;
  //init_engine(level1);
  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_protocol = 0;
  hints.ai_flags=AI_PASSIVE|AI_ADDRCONFIG;
  if(getaddrinfo(NULL, "8888", &hints, &res)!=0){
    printf("Coudln't get address info\n");
    /*Error*/
  }
  if((sock=socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1){
    printf("Couldn't open socket\n");
    /*Error*/
  }
  if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int))==-1){
    perror("setsockopt");
    exit(1);
  }
  if(bind(sock, res->ai_addr, res->ai_addrlen) == -1){
    printf("ERROR Coudln't bind socket\n");
    perror("Bind failed");
    exit(1);
    /*Couldn't bind socket*/
  }
  freeaddrinfo(res);
  while(1){
    memset(buf, 0, sizeof(buf));
    count = recvfrom(sock, buf, sizeof(buf),0, &client_addr, &src_addr_len);
    if(count>512){
      count = 512;
    }
    server_handle_datagram(buf, &client_addr, count);
  }
  return 0;
}

int net_join_server(char *address, int port){
  char *buf;
  buf = malloc(sizeof(char));
  buf[0] = NET_JOIN;
  struct addrinfo hints, *res;
  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_protocol = 0;
  hints.ai_flags=AI_PASSIVE|AI_ADDRCONFIG;
  if(getaddrinfo(NULL, "8888", &hints, &res)!=0){
    /*Error*/
  }
  if((server_socket=socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1){
    /*Error*/
  }
  sendto(server_socket, buf, sizeof(buf), 0, res->ai_addr, res->ai_addrlen);
  freeaddrinfo(res);
  return 0;
}


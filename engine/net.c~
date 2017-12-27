#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include "net.h"

net_message_node *_net_out_head = NULL;
net_message_node *_net_out_current = NULL;
bool _net_on = true;

void clear_messages();

void net_init(){
  _net_out_head = malloc(sizeof(net_message_node));
  _net_out_current = _net_out_head;
}

void net_add_message(net_message *msg){
  memcpy(&(_net_out_current->msg), msg, sizeof(net_message));
  _net_out_current->next = malloc(sizeof(net_message_node));
  _net_out_current = _net_out_current->next;
  _net_out_current->next = NULL;
}

void net_flush_messages(){
  net_message_node *current = _net_out_head;
  while(current){
    net_message *msg = &current->msg;
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

void net_handle_messages(){
  return;
}



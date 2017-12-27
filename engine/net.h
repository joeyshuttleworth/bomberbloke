#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define NET_BUFFER_SIZE 513
#define DEFAULT_PORT 8888

enum opcode{
  NET_SYN,
  NET_ACK,
  NET_SYN_ACK,
  NET_CREATE,
  NET_DESTROY,
  NET_MOVE,
  NET_FULL_SYNC,
  NET_SYNC_COMPLETE,
  NET_JOIN,
  NET_LEAVE,
  NET_KICK,
  NET_MSG
};

typedef struct{
  struct sockaddr address;
  enum opcode operation;
  char *data;
  unsigned int data_size;  
} net_message;

struct net_message_node{
  net_message *msg;
  struct net_message_node *next;
};

typedef struct net_message_node net_message_node;

extern net_message_node *_net_out_current;
extern net_message_node *_net_out_head;
extern bool _net_on;
extern struct sockaddr_in _server_address;
extern int server_socket;

void net_flush_messages();
void net_add_message(net_message*);
void net_handle_messages();
void net_messages_init();
void net_init();
void *receive_loop(void*);
net_message *net_get_message();
void server_handle_datagram(char*, struct sockaddr*, int);

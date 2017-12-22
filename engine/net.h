#include <stdbool.h>
#define NET_BUFFER_SIZE 64

enum opcode{
  NET_SYN,
  NET_ACK,
  NET_SYN_ACK,
  NET_CREATE,
  NET_DESTROY,
  NET_MOVE,
  NET_FULL_SYNC,
  NET_SYNC_COMPLETE
};

typedef struct{
  enum opcode operation;
  char *data;
  unsigned int data_size;  
} net_message;

struct net_message_node{
  net_message msg;
  struct net_message_node *next;
};

typedef struct net_message_node net_message_node;

extern net_message_node *_net_out_current;
extern net_message_node *_net_out_head;
extern bool _net_on;

void net_flush_messages();
void net_add_message(net_message*);
void net_handle_messages();
void net_init();

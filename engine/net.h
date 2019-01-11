#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>
#include <float.h>
#include <math.h>

#define NET_BUFFER_SIZE 513
#define DEFAULT_PORT 8888
#define DEFAULT_ATTEMPTS 5
#define NET_RATE 1
#define TICK_RATE 1
#define MAX_ATTEMPTS 65535

enum opcode{
      NET_PING,
      NET_SYN,
      NET_PAUSE,
      NET_UNPAUSE,
      NET_ACK,
      NET_ERROR,
      NET_MOVE,
      NET_JOIN,
      NET_LEAVE,
      NET_KICK,
      NET_MSG,
      NET_START,
      NET_ACTOR_LIST,
      NET_NEW_GAME
};

typedef struct{
  struct sockaddr_storage address;
  enum opcode operation;
  char *data;
  uint8_t address_length;
  uint16_t data_size;
  uint8_t id;
  uint8_t frequency;
  uint16_t attempts;
  uint8_t offset;
} net_message;

typedef struct net_message_node{
  net_message *msg;
  struct net_message_node *next;
} net_message_node;

typedef struct net_float{
  bool sign : 1;                           /*See IEEE 754*/
  unsigned long int mantissa : 52;
  int exponent : 11;
} net_float;

typedef struct net_move{
  uint16_t id;
  net_float position[2];
  net_float velocity[2];
} net_move;
  
extern net_message_node *_net_out_current;
extern net_message_node *_net_out_head;
extern bool _net_on;
extern int _server_socket;
extern struct sockaddr_storage _server_address;
extern uint8_t _current_id;
extern unsigned int _tick;

void net_flush_messages();
void net_add_message(net_message*);
void net_handle_messages();
void net_messages_init();
void net_init();
void net_float_create(double, net_float*);
void *receive_loop(void*);
net_message_node *net_get_message(unsigned int);
void handle_datagram(char*, struct sockaddr_storage*, unsigned int, unsigned int);
void net_join_server(const char*, char*, const char*);
void net_remove_message(net_message_node*);
void timeout(net_message_node*);
void net_remove_messages_to(struct sockaddr_storage*); 
void net_clear_messages();
void net_send_message();
extern pthread_mutex_t net_out_mutex;

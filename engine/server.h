#include <list>
#include "engine.h"
#include "player.h"
extern "C"{
#include "net.h"
}
#define BILLION  1000000000L

void server_loop();
void server_handle_datagram(char*, struct sockaddr*, int);
extern unsigned int _ping_time;

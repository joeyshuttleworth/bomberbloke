#include <list>
#include <pthread.h>
#include "engine.h"
#include "player.h"
extern "C"{
#include "net.h"
}

void server_loop();
void server_handle_datagram(char*, struct sockaddr*, int);

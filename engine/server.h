#include <list>
#include <pthread.h>

void server_loop();
void server_handle_datagram(char*, struct sockaddr*, int);

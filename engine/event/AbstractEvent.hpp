/* Abstract class for events to be sent to the client/server.*/

#include <enet/enet.h>

#ifndef ABSTRACTEVENT_HPP

#define ABSTRACTEVENT_HPP

typedef enum EventType{
  join,
  clientmessage,
  servermessage,
  sync,
  newgame,
  start,
  stop
};

class AbstractEvent{
public:
  
private:
  int mType;
};


#endif

/* Abstract class for events to be sent to the client/server.*/

#include <enet/enet.h>

#ifndef ABSTRACTEVENT_HPP

#define ABSTRACTEVENT_HPP

enum EventType{
  JOIN,
  CLIENT_MESSAGE,
  SERVER_MESSAGE,
  CREATE,
  REMOVE,
  SYNC,
  NEW_GAME,
  START,
  STOP
};

class AbstractEvent{
public:
  
private:
  int mType;
};


#endif

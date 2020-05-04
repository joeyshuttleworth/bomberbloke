/* Event that describes how an actor has moved */

#ifndef MOVEEVENT_HPP

#define MOVEEVENT_HPP

class MoveEvent : AbstractEvent{
private:
  actor *mpActor;
public:
  MoveEvent(actor*);
  ~MoveEvent();
  MoveEvent(&MoveEvent);
}

#endif

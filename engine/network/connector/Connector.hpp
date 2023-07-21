#ifndef CONNECTOR_HPP
#define CONNECTOR_HPP

#include <map>
#include <list>
#include <AbstractEvent.hpp>
#include <AbstractPlayer.hpp>

/*
 * Event object that the Connector interface emits
 */

struct EventReceived {
  std::shared_ptr<AbstractEvent> event;
  int from_id;
};

/*
 * Connector Interface
 *
 * All client/server code is intended to target this interface, so that the
 * underlying network code is abstracted away.
 */

class Connector
{
protected:
  virtual int nextFreeId() = 0;

public:
  virtual ~Connector() = default;
  virtual void configure(ushort port) = 0;
  virtual void open() = 0;
  virtual void close() = 0;

  // We provide an interface for smart pointers
  void sendEvent(std::unique_ptr<AbstractEvent> event, int to_id)
  {
    std::shared_ptr<AbstractEvent> s_event = std::move(event);
    sendEvent(s_event, to_id);
  }
  virtual void sendEvent(std::shared_ptr<AbstractEvent> event, int to_id) = 0;

  virtual int connectPeer(std::string address, short port) = 0;
  virtual void disconnectPeer(int id, std::string reason) = 0;

  virtual int statRoundTripTime(int id) = 0;

  std::list<EventReceived> cache;
  virtual std::list<EventReceived> poll(
    int) = 0;
  virtual EventReceived pollFor(
    int timeout,
    std::set<EventType>& lookFor) = 0;

  virtual int countPeers() = 0;

  void broadcastEvent(std::unique_ptr<AbstractEvent> event)
  {
    std::shared_ptr<AbstractEvent> s_event = std::move(event);
    broadcastEvent(s_event);
  }
  virtual void broadcastEvent(std::shared_ptr<AbstractEvent> event) = 0;
};

#endif
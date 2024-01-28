#pragma once

#include <list>
#include <set>
#include <AbstractEvent.hpp>

typedef int ConnectorPeer;

/*
 * Connector interface speaks in AbstractEvent, below is a struct emitted when receieving one over network
 */
struct EventReceived {
  std::shared_ptr<AbstractEvent> event;
  ConnectorPeer from;

  bool operator==(EventReceived& rhs) {
    return event == rhs.event && from == rhs.from; 
  }
  bool operator!=(EventReceived& rhs) {
    return !(*this == rhs);
  }
};
static EventReceived EVENT_RECEIVED_NONE = { nullptr , -1 };

/*
* Recomended way of creating a Connector object.
*/
enum ConnectorMode {
  CLIENT,
  SERVER
};
struct ConnectorDescriptor {
  ConnectorMode mode;

  // mode == CLIENT
  std::string serverAddress = "";
  short serverPort = 0;

  // mode == SERVER
  short listenPort = 0;
};
class Connector;
std::unique_ptr<Connector> createConnector(ConnectorDescriptor desc);

/*
 * Connector Interface
 *
 * All network stuff by Client/Server is supposed to use this interface.
 * Left suitably general to hide away protocol specific details.
 */

class Connector
{
public:
  virtual ~Connector() = default;

  /*
  * open() must be done once, to begin network activity, and be followed by a corresponding close()
  */
  virtual bool open() = 0;

  /*
  * close() must follow a single open(), and ceases any network activity
  */
  virtual void close() = 0;

  /*
  * sendEvent with automatic discarding of event after completion
  */
  void sendEvent(std::unique_ptr<AbstractEvent> event, ConnectorPeer to_id)
  {
    std::shared_ptr<AbstractEvent> s_event = std::move(event);
    sendEvent(s_event, to_id);
  }

  /*
  * sendEvent transmits an AbstractEvent to a ConnectorPeer
  */
  virtual void sendEvent(std::shared_ptr<AbstractEvent> event, ConnectorPeer to_id) = 0;

  /*
  * disconnectPeer drops a ConnectorPeer, invalidating it, and may also send a KickEvent to them
  */
  virtual void disconnectPeer(ConnectorPeer id, std::string reason) = 0;

  /*
  * latency provides a relative metric of latency for a given peer, implementation defined
  */
  virtual int latency(ConnectorPeer id) = 0;

  /*
  * poll returns a list, if any, of EventReceived in 'chronological' order, Connector will no longer track these events internally
  */
  virtual std::list<EventReceived> poll(
    int) = 0;
  
  /*
  * pollFor spends a maximum of _timeout_ miliseconds waiting for at most one of EventType from any peer,
  * if none received then EventRecieved = { nullptr , -1 }
  */
  virtual EventReceived pollFor(
    int timeout,
    std::set<EventType>& lookFor) = 0;

  /*
  * Returns number of currently valid ConnectorPeer's
  */
  virtual int countPeers() = 0;

  /*
  * broadcastEvent with automatic discarding of AbstractEvent after completion 
  */
  void broadcastEvent(std::unique_ptr<AbstractEvent> event)
  {
    std::shared_ptr<AbstractEvent> s_event = std::move(event);
    broadcastEvent(s_event);
  }

  /*
  * Broadcasts event to each currently valid ConnectorPeer once.
  */
  virtual void broadcastEvent(std::shared_ptr<AbstractEvent> event) = 0;
};

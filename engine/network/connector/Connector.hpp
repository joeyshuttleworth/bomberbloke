#ifndef CONNECTOR_HPP
#define CONNECTOR_HPP

#include <map>
#include <list>
#include <AbstractEvent.hpp>
#include <AbstractPlayer.hpp>


class Connector {
protected:
  std::map<int, std::shared_ptr<void>> peers; // (id, *peer)

  ushort mPort;

  int findIdFromPeer(void *peer) {
    for(auto pair : peers) {
      if(pair.second.get() == peer)
        return pair.first;
    }
    return -1;
  }
  int nextFreeId() {
    int id = 0;
    for(const auto& peer : peers) {
      if(peer.first > id)
        id = peer.first+1;
    }
    return id;
  }

public:
  virtual ~Connector() = 0;

  virtual void configure(ushort port) = 0;
  virtual void open() = 0;
  virtual void close() = 0;

  // We provide an interface for smart pointers
  void sendEvent(std::unique_ptr<AbstractEvent> &event, int to_id) {
    sendEvent(*event, to_id);
  }
  void sendEvent(std::shared_ptr<AbstractEvent> &event, int to_id) {
    sendEvent(*event, to_id);
  }
  virtual void sendEvent(AbstractEvent &event, int to_id) = 0;
  virtual int connectPeer(std::string address, short port) = 0;
  virtual void disconnectPeer(int id, std::string reason) = 0;
  virtual std::list<std::pair<int, std::shared_ptr<AbstractEvent>>> poll(int) = 0;

  int countPeers() {
    return (int) peers.size();
  }

  void broadcastEvent(std::unique_ptr<AbstractEvent> &event) {
    broadcastEvent(*event);
  }
  void broadcastEvent(std::shared_ptr<AbstractEvent> &event) {
    broadcastEvent(*event);
  }
  void broadcastEvent(AbstractEvent &event) {
    for(auto peer : peers)
      sendEvent(event, peer.first);
  }
};

#endif
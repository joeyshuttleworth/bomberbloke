#ifndef CONNECTOR_HPP
#define CONNECTOR_HPP

#include <map>
#include <list>
#include <AbstractEvent.hpp>
#include <AbstractPlayer.hpp>


class Connector {
protected:
  std::map<int, std::shared_ptr<void>> peers; // (id, *peer)

  // Listening on
  std::string mAddress;
  int mPort;

  int findIdFromPeer(void *peer) {
    for(auto pair : peers) {
      if(pair.second.get() == peer || peer.)
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
  Connector();
  Connector(int port) {
    mPort = port;
  }
  ~Connector();

  virtual void sendEvent(std::shared_ptr<AbstractEvent> event, int to_id);
  virtual void disconnectPeer(int id, std::string reason);
  virtual std::list<std::pair<int, std::shared_ptr<AbstractEvent>>> poll();

  int countPeers() {
    return (int) peers.size();
  }

  void broadcastEvent(std::shared_ptr<AbstractEvent> event) {
    if(countPeers() == 0) {
      log_message(ERR, "No Clients Connected, Could not Broadcast Message!");
      return;
    }
    for(auto peer : peers)
      sendEvent(event, peer.first);
  }
};

#endif
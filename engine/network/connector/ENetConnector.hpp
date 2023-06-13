#ifndef ENETCONNECTOR_HPP
#define ENETCONNECTOR_HPP

#include <Connector.hpp>

class ENetConnector : public Connector {
protected:
  std::map<int, std::shared_ptr<ENetPeer>> peers; // (id, *peer)
  ENetHost *mENetHost = nullptr;

  // Listening on
  std::string mAddress;
  int mPort;
  ENetAddress mENetAddress;

public:
  ENetConnector();
  ENetConnector(int port);
  ~ENetConnector();

  void sendEvent(std::shared_ptr<AbstractEvent> event, int to_id);
  void disconnectPeer(int id, std::string reason);
  std::list<std::pair<int, std::shared_ptr<AbstractEvent>>> poll();

  void sendPacket(ENetPeer* peer, ENetPacket* packet, enet_uint8 channel);
};

#endif
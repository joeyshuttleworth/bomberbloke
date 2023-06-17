#ifndef ENETCONNECTOR_HPP
#define ENETCONNECTOR_HPP

#include <Connector.hpp>

class ENetConnector : public Connector {
protected:
  std::map<int, std::shared_ptr<ENetPeer>> peers; // (id, *peer)
  ENetHost *mENetHost = nullptr;

  // Listening on, will typically remain un-configured for client use
  bool mAddressConfigured = false;
  ENetAddress mENetAddress;

  void sendPacket(ENetPeer* peer, ENetPacket* packet, enet_uint8 channel);

public:
  ENetConnector();
  ~ENetConnector();

  void configure(ushort port) override;
  void open() override;
  void close() override;

  using Connector::sendEvent;
  void sendEvent(AbstractEvent &event, int to_id) override;
  int connectPeer(std::string address, short port) override;
  void disconnectPeer(int id, std::string reason) override;
  std::list<std::pair<int, std::shared_ptr<AbstractEvent>>> poll(int) override;
};

#endif
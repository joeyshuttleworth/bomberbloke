#ifndef ENETCONNECTOR_HPP
#define ENETCONNECTOR_HPP

#include <Connector.hpp>
#include <enet/enet.h>


class ENetConnector : public Connector
{
protected:
  std::map<int, ENetPeer*> peers; // (id, *peer)
  ENetHost* mENetHost = nullptr;
  short mPort;

  int nextFreeId() override;

  // Listening on, will typically remain un-configured for client use
  bool mAddressConfigured = false;
  ENetAddress mENetAddress;

  void sendPacket(ENetPeer* peer, ENetPacket* packet, enet_uint8 channel);
  int findIdFromPeer(ENetPeer* peer);

public:
  ENetConnector();
  ~ENetConnector();

  void configure(ushort port) override;
  void open() override;
  void close() override;

  using Connector::sendEvent;
  void sendEvent(std::shared_ptr<AbstractEvent> event, int to_id) override;

  void parseENetPacket(ENetEvent& event,
                       std::shared_ptr<AbstractEvent>& abstractEvent);

  int connectPeer(std::string address, short port) override;
  void disconnectPeer(int id, std::string reason) override;
  std::list<std::pair<int, std::shared_ptr<AbstractEvent>>> poll(int) override;
  std::pair<int, std::shared_ptr<AbstractEvent>> pollFor(
    int timeout,
    std::set<EventType>& lookFor) override;

  int countPeers() override;
  void broadcastEvent(std::shared_ptr<AbstractEvent> event) override;
};

#endif

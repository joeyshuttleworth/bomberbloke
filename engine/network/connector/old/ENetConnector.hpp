#ifndef ENETCONNECTOR_HPP
#define ENETCONNECTOR_HPP

#include <Connector.hpp>

#ifdef ENET
#include <enet/enet.h>
#endif


class ENetConnector : public Connector
{
protected:
  std::map<int, ENetPeer*> peers; // (id, *peer)
  ENetHost* mENetHost = nullptr;
  short mPort;

  int nextFreeId();

  // Listening on, will typically remain un-configured for client use
  bool mAddressConfigured = false;
  ENetAddress mENetAddress;

  void sendPacket(ENetPeer* peer, ENetPacket* packet, enet_uint8 channel);
  int findIdFromPeer(ENetPeer* peer);

  void processENetEvent(ENetEvent &event,
                        std::shared_ptr<AbstractEvent> &emitted_event,
                        int &from_id);

public:
  ENetConnector();
  ~ENetConnector();

  void configureListenPort(ushort port) override;
  void open() override;
  void close() override;

  using Connector::sendEvent;
  void sendEvent(std::shared_ptr<AbstractEvent> event, int to_id) override;

  int connectPeer(std::string address, short port) override;
  void disconnectPeer(int id, std::string reason) override;

  int statRoundTripTime(int id) override;

  std::list<EventReceived> poll(int) override;
  EventReceived pollFor(
    int timeout,
    std::set<EventType>& lookFor) override;

  int countPeers() override;
  void broadcastEvent(std::shared_ptr<AbstractEvent> event) override;
};

#endif

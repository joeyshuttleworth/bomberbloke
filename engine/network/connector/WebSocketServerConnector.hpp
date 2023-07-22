//
// Created by Harvey Williams on 16/07/2023.
//

#ifndef BOMBERBLOKE_LIBWSCONNECTOR_H
#define BOMBERBLOKE_LIBWSCONNECTOR_H

#include "Connector.hpp"

class libWSConnector : Connector
{
protected:
  std::map<int, void*> peers; // (id, *peer)
  short mPort;

  int nextFreeId() override;

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

#endif // BOMBERBLOKE_LIBWSCONNECTOR_H

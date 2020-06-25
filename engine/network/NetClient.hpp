//
// Created by dave on 08.06.20.
//

#ifndef NETCLIENT_HPP
#define NETCLIENT_HPP


#include <enet/enet.h>
#include <string>
#include <memory>
#include "AbstractEvent.hpp"


class serverPlayer;

class NetClient {
public:
    NetClient();

    ~NetClient();
    bool connectClient(std::string serverAddress = "127.0.0.1", enet_uint16 = 8888);
    void sendStringMessage(std::string message);
    void disconnectClient();
    bool isConnected();
    void pollServer();
    bool joinBlokeServer(std::string address, int, std::string);
    void sendEvent(std::unique_ptr<AbstractEvent>&);

    std::vector<serverPlayer> mPlayers;

private:
  /* And ENet representation of the address of the server */
    ENetAddress mENetServerAddress;
    ENetPeer *mENetServer;
  /** The ENet host used by this client   */
    ENetHost *mENetHost = nullptr;

    std::string mServerAddress;
    enet_uint16 mPort;
};



class NetClient;


#endif

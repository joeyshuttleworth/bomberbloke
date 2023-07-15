//
// Created by dave on 08.06.20.
//

#ifndef NETCLIENT_HPP
#define NETCLIENT_HPP

#include <string>
#include <memory>
#include "AbstractEvent.hpp"
#include "ENetConnector.hpp"


class serverPlayer;

class NetClient {
public:
    NetClient();
    ~NetClient();

    bool connectClient(std::string serverAddress = "127.0.0.1", ushort = 8888);
    void disconnectClient();
    bool isConnected();
    void pollServer();
    bool joinBlokeServer(std::string address, const std::string&, const std::vector<std::string>& = {});
    void handleServerCommand(std::string);

    std::vector<serverPlayer> mPlayers;

    std::string mServerAddress;
    short mPort;
    int mServerId;

    std::unique_ptr<Connector> mConnector;
};

#endif

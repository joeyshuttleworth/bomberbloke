//
// Created by dave on 08.06.20.
//

#ifndef BOMBERBLOKE_CLIENT_H
#define BOMBERBLOKE_CLIENT_H

#endif //BOMBERBLOKE_CLIENT_H

#include <enet/enet.h>
#include <string>

class NetClient {
public:
    NetClient();

    ~NetClient();
    void connectClient(std::string serverAddress, enet_uint16 port);

    void disconnectClient();
    bool isConnected();

private:
    ENetAddress address;
    ENetEvent event;
    ENetPeer *peer;
    ENetHost *host = nullptr;

    std::string serverAddress = "127.0.0.1";
    enet_uint16 port = 8888;
};

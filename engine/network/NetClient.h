//
// Created by dave on 08.06.20.
//

#ifndef BOMBERBLOKE_CLIENT_H
#define BOMBERBLOKE_CLIENT_H

#endif //BOMBERBLOKE_CLIENT_H

#include <enet/enet.h>
#include <string>

class Client {
    Client();

    ~Client();

public:
    void connectClient(std::string serverAddress, enet_uint16 port);

    void disconnectClient();

    enet_uint32 rttTime();

private:
    ENetHost *host = nullptr;
    ENetPeer *peer = nullptr;
    std::string serverAddress = "localhost";
    enet_uint16 port = 8888;
};
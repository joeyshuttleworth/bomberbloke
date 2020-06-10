//
// Created by dave on 08.06.20.
//

#ifndef BOMBERBLOKE_SERVER_H
#define BOMBERBLOKE_SERVER_H
#include <string>
#include <enet/enet.h>
#include "ServerInfoEvent.hpp"

class Server {
public:
    uint32_t clientCount() const;

    enet_uint32 packetsSent() const;

    enet_uint32 packetsRecieved() const;

    Server();

    ~Server();

    bool status();

    void disconnectPeer(ENetPeer *peer);

    void broadcastPacket(ENetPacket *packet, enet_uint8 channel = 0);

    void sendPacket(ENetPeer *peer, ENetPacket *packet, enet_uint8 channel = 0);

    void poll();

    /* Update the master server about us. If disconnect is true, we will disconnect from the master server
       otherwise, we will send our info to the master server.
     */
    void updateGameMasterServer(bool disconnect);

    void removeFromMasterServer();

private:
    ENetHost *server = nullptr;
    ENetAddress address;
    ENetEvent event;
    ENetPacket *packet;
    std::string mMasterServerAddress;

    bool stop();

    bool init_enet();

    enet_uint16 PORT = 4500;

};
#endif

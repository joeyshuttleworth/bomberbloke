//
// Created by dave on 08.06.20.
//

#ifndef BOMBERBLOKE_SERVER_H
#define BOMBERBLOKE_SERVER_H

#endif //BOMBERBLOKE_SERVER_H

#include <enet/enet.h>
#include <string>

class NetServer {
public:
    uint32_t clientCount() const;

    enet_uint32 packetsSent() const;

    enet_uint32 packetsRecieved() const;

    NetServer();

    ~NetServer();

    bool status();

    void disconnectPeer(ENetPeer *peer);

    void broadcastPacket(ENetPacket *packet, enet_uint8 channel = 0);

    void sendPacket(ENetPeer *peer, ENetPacket *packet, enet_uint8 channel = 0);

    void poll();
    bool isConnected();
private:
    ENetHost *server = nullptr;
    ENetAddress address;
    ENetEvent event;
    ENetPacket *packet;

    bool stop();

    bool init_enet();
    void updateGameMasterServer(bool disconnect);
    enet_uint16 PORT = 8888;
    std::string masterServerAddress = "http://ptsv2.com/t/faryp-1591787919/post";

};

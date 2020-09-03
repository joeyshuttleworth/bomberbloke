//
// Created by dave on 08.06.20.
//

#ifndef NETSERVER_HPP
#define NETSERVER_HPP
#include <string>
#include <enet/enet.h>
#include <string>
#include "ServerInfo.hpp"

class AbstractEvent;
class JoinEvent;

typedef struct unjoinedPeer{
  ENetPeer* peer;
  unsigned int timeout = 600;
} unjoinedPeer;

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


    //! Poll and Handle all ENetEvents in the queue, by default will wait one second for an event before processing
    void poll();

    //! Keeps polling until the _halt flag is set to true. This is used to have the server running in a separate thread.
    void pollLoop();

    /* Update the master server about us. If disconnect is true, we will disconnect from the master server
       otherwise, we will send our info to the master server.
     */

    void removeFromMasterServer();

    bool isConnected();

    bool init_enet();

    void handleJoinEvent(std::shared_ptr<JoinEvent> event, ENetPeer *from);

    bool stop();

    void update();

    void sendEvent(std::unique_ptr<AbstractEvent>&, ENetPeer *peer);
    void broadcastEvent(std::unique_ptr<AbstractEvent>&);
    void syncPlayerProperties(std::shared_ptr<AbstractPlayer> player);
    void syncPlayers();

    void flush();
private:
    ENetHost *mENetServer = nullptr;
    ENetAddress mENetAddress;
    std::string mMasterServerAddress;
    void sendStringMessage(std::string, ENetPeer*);
    void handleJoinEvent();

    ServerInfo mServerInfo;

    /** handleEvent
     *  Called when the server receives some kind of event.
     *  @param pointer to an event that has been received
     */
  void handleEvent(std::shared_ptr<AbstractEvent>, ENetPeer*);

  std::list<unjoinedPeer> mUnjoinedPeers;

  void updateGameMasterServer(bool disconnect);
  enet_uint16 mPort = 8888;
  std::string masterServerAddress = "http://ptsv2.com/t/faryp-1591787919/post";

};
#endif

//
// Created by dave on 08.06.20.
//

#ifndef NETSERVER_HPP
#define NETSERVER_HPP
#include <string>
#include <enet/enet.h>
#include <string>
#include "ServerInfo.hpp"
#include "AbstractPlayer.hpp"

class AbstractEvent;
class JoinEvent;

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

  /*
   * This is called every tick to ping players, remove timed out players
   * and flush the message queue.
   */
    void update();

    void sendEvent(std::unique_ptr<AbstractEvent>&, ENetPeer *peer);
    void broadcastEvent(std::unique_ptr<AbstractEvent>&);
    void syncPlayerProperties(std::shared_ptr<AbstractPlayer> player);
    void syncPlayers();

    void printPlayers();

    void disconnectPlayer(std::shared_ptr<AbstractPlayer>, std::string="", bool=false);

    void disconnectPlayer(std::string player_name, std::string reason="");

    void handlePlayerLeave(std::shared_ptr<AbstractPlayer>);
private:

  /** A list containing information about every player connected to the server
   * including local and network players
   */
  std::list<std::shared_ptr<AbstractPlayer>> mPlayerList;

  /* Poll and Handle all ENetEvents in the queue, by default will wait one second
   * for an event before processing
   */
    void poll();
    /*
    * Send all of the ENet messages which are waiting to be sent.
    */
    void flush();
    ENetHost *mENetServer = nullptr;
    ENetAddress mENetAddress;
    std::string mMasterServerAddress;
    void sendStringMessage(std::string, ENetPeer*);
    void handleJoinEvent();

    /* Gives the player a unique id and adds them to the player list provided that
     * no other player has the same address.
     */
  bool addPlayer(std::shared_ptr<AbstractPlayer>);

    ServerInfo mServerInfo;

    /** handleEvent
     *  Called when the server receives some kind of event.
     *  @param pointer to an event that has been received
     */
  void handleEvent(std::shared_ptr<AbstractEvent>, ENetPeer*);

  void updateGameMasterServer(bool disconnect);
  enet_uint16 mPort = 8888;
  std::string masterServerAddress = "http://ptsv2.com/t/faryp-1591787919/post";

};

#endif

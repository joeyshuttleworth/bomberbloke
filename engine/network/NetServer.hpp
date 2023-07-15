//
// Created by dave on 08.06.20.
//

#ifndef NETSERVER_HPP
#define NETSERVER_HPP
#include "AbstractPlayer.hpp"
#include <connector/ENetConnector.hpp>
#include "ServerInfo.hpp"
#include <enet/enet.h>
#include <list>
#include <string>

class AbstractEvent;
class JoinEvent;
class CommandEvent;

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


    /* Update the master server about us. If disconnect is true, we will disconnect from the master server
       otherwise, we will send our info to the master server.
     */

    void removeFromMasterServer();
    bool isConnected();

    void init(int);
    void handleJoinEvent(std::shared_ptr<JoinEvent> event, int from_id);
    void handleCommandEvent(std::shared_ptr<CommandEvent>, int from_id);
    bool stop();
    void setMasterServerAddress(std::string masterServerAddress);
    void update();
    void broadcastEvent(std::unique_ptr<AbstractEvent> event);
    void syncPlayerProperties(std::shared_ptr<AbstractPlayer> player);
    void syncPlayers();
    void printPlayers();
    void disconnectPlayer(std::shared_ptr<AbstractPlayer>, std::string="");
    void disconnectPlayer(const std::string& player_name, std::string reason="");
    void handlePlayerLeave(const std::shared_ptr<AbstractPlayer>&);

    /** handleEvent
     *  Called when the server receives some kind of event.
     *  @param pointer to an event that has been received
     */
    void handleEvent(std::shared_ptr<AbstractEvent>, int from_id);

    std::unique_ptr<Connector> mConnector;
private:

  /** A list containing information about every player connected to the server
   * including local and network players
   */
  std::list<std::shared_ptr<AbstractPlayer>> mPlayerList;

  // TODO It uses _player_list rather than above, why?
  std::shared_ptr<AbstractPlayer> findPlayer(int id);

  void sendStringMessage(std::string, ENetPeer*);
  void handleJoinEvent();

  ServerInfo mServerInfo;

  void updateGameMasterServer(bool disconnect);
  std::string mMasterServerAddress;
};

#endif

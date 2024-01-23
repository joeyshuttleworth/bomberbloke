//
// Created by dave on 08.06.20.
//

#ifndef NETSERVER_HPP
#define NETSERVER_HPP
#include "AbstractPlayer.hpp"
#include "ServerInfo.hpp"
#include "Connector.hpp"
#include <list>
#include <string>

class AbstractEvent;
class JoinEvent;
class CommandEvent;

class NetServer {
public:
    NetServer();

    ~NetServer();

    /* Update the master server about us. If disconnect is true, we will disconnect from the master server
       otherwise, we will send our info to the master server.
     */
    void removeFromMasterServer();

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

    std::unique_ptr<Connector> mConnector;
private:

  /** A list containing information about every player connected to the server
   * including local and network players
   */
  std::list<std::shared_ptr<AbstractPlayer>> mPlayerList;

  // TODO It uses _player_list rather than above, why?
  std::shared_ptr<AbstractPlayer> findPlayer(int id);

  /** handleEvent
     *  Called when the server receives some kind of event.
     *  @param Event to process
     *  @param int id, specified by Connector, of the sender
   */
  void handleEvent(std::shared_ptr<AbstractEvent>, int from_id);

  ServerInfo mServerInfo;

  void updateGameMasterServer(bool disconnect);
  std::string mMasterServerAddress;
};

#endif

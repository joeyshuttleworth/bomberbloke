//
// Created by dave on 08.06.20.
//
#include "NetServer.hpp"
#include "AbstractEvent.hpp"
#include "CommandEvent.hpp"
#include "JoinEvent.hpp"
#include "MessageEvent.hpp"
#include "PlayerPropertiesEvent.hpp"
#include "QueryEvent.hpp"
#include "ServerInfoEvent.hpp"
#include "acceptEvent.hpp"
#include "engine.hpp"
#include "errorEvent.hpp"
#include "syncEvent.hpp"
#include <curl/curl.h>
#include <sstream>
#include <string>

NetServer::NetServer()
{
  // init();
}

NetServer::~NetServer()
{
  // Clean up ENet
  stop();
}

void
NetServer::pollLoop()
{
  while (!_halt) {
    poll();
  }
  return;
}

void
NetServer::handleJoinEvent(std::shared_ptr<JoinEvent> event, int from_id)
{
  std::string nickname = event->mNickname;

  /*  First check that there's space on the server */
  if (_server_info.mMaxPlayers <= _player_list.size()) {
    return;
  }
  /* Second check that no player has the same nickname */
  auto iter = find_if(_player_list.begin(),
                      _player_list.end(),
                      [&](std::shared_ptr<AbstractPlayer> p) {
                        return nickname == p->mNickname;
                      });

  if (iter != _player_list.end()) {
    return;
  }
  /* Add the player to _player_list */
  auto player = std::make_shared<NetworkPlayer>(nickname, from_id);
  player->setId(from_id);
  _player_list.push_back(player);

  /* Send an acceptEvent */
  std::shared_ptr<AbstractEvent> accept_event(new acceptEvent());
  mConnector.sendEvent(accept_event, from_id);
  /* Now sync send the entire gamestate to the client */
  std::shared_ptr<AbstractEvent> s_event(new syncEvent(from_id));
  mConnector.sendEvent(s_event, from_id);

  // Handle join commands e.g 'colour 0x0000ffff'
  for(auto& command : event->getCommands()){
    handleCommandEvent(std::make_shared<CommandEvent>(command), from_id);
  }

  return;
}

std::shared_ptr<AbstractPlayer>
NetServer::findPlayer(int id) {
  for(auto i = _player_list.begin(); i != _player_list.end(); i++) {
    if((int) (*i)->getId() == id)
      return (*i);
  }
  return nullptr;
}

void
NetServer::handleEvent(std::shared_ptr<AbstractEvent> pEvent, int from_id)
{
  if (!pEvent.get()) {
    log_message(ERR, "tried to handle a null event");
    return;
  }

  switch (pEvent->getType()) {
    case EVENT_QUERY: {
      /*  Get the server info and see if the username may be used */
      std::shared_ptr<QueryEvent> pquery_event =
        std::dynamic_pointer_cast<QueryEvent>(pEvent);
      std::shared_ptr<AbstractEvent> info_event(new ServerInfoEvent(
        mServerInfo, _player_list, pquery_event->getNickname()));
      mConnector.sendEvent(info_event, from_id);
      break;
    }
    case EVENT_JOIN: {
      std::shared_ptr<JoinEvent> pjoin_event =
        std::dynamic_pointer_cast<JoinEvent>(pEvent);

      if(findPlayer(from_id) != nullptr) {
        log_message(INFO, "Player with ID " + std::to_string(from_id) +
                            "is already connected");
        std::shared_ptr<AbstractEvent> e_event(
          new errorEvent("Player from host already connected"));
        mConnector.sendEvent(e_event, from_id);
      }
      handleJoinEvent(pjoin_event, from_id);
      break;
    }
    case EVENT_COMMAND: {
      std::shared_ptr<CommandEvent> p_command_event =
        std::dynamic_pointer_cast<CommandEvent>(pEvent);
      handleCommandEvent(p_command_event, from_id);
      break;
    }
    default:
      break;
  }
  return;
}

void
NetServer::handleCommandEvent(std::shared_ptr<CommandEvent> c_event, int from_id)
{
  std::shared_ptr<AbstractPlayer> p_player = findPlayer(from_id);
  if (!p_player) {
    log_message(INFO, "Command event received from non-connected player");
    return;
  }
  std::string command = c_event->getCommand();
  log_message(DEBUG, "received command " + command + " from player " +
  std::to_string(p_player->getId()) +  ".");

  if (command != "") {
    auto tokens = split_to_tokens(command);

    if(tokens.front() == "colour"){
      // Player has requested to change colour
      if(tokens.size()!=2)
        log_message(INFO, "colour command requires exactly one argument");
      else{
        // Read in colour as hex
        std::stringstream colour_stream;
        colour_stream << std::hex << tokens.back();
        uint32_t colour;
        colour_stream >> colour;
        colour |= 0xFF;
        // Change the colour as requested
        p_player->setColour(colour);
        log_message(DEBUG,
                    "Setting player " + std::to_string(p_player->getId()) + " to " + std::to_string(colour));

      }
    }

      auto character = p_player->getCharacter();
      if (!character) {
        log_message(INFO, "Command received from player without character");
        return;
      }
      // Send command to players character
      character->handleCommand(command);
    }
  return;
}

//! Initialise ENet, and create server instance.
// Returns: false if failed to create server
void
NetServer::init()
{
  mConnector = ENetConnector(mPort);
}

bool
NetServer::stop()
{
  // TODO: Disconnect clients gracefully
  updateGameMasterServer(true);
  return 0;
}

void
NetServer::setMasterServerAddress(std::string masterServerAddress)
{
  mMasterServerAddress = masterServerAddress;
}

void
NetServer::removeFromMasterServer()
{
  updateGameMasterServer(true);
  // printf("%s disconnected.\n", event.peer->data);
}

void
NetServer::updateGameMasterServer(bool disconnect)
{
  CURL* curl;
  CURLcode res;
  if (!disconnect) {
    /* In windows, this will init the winsock stuff */
    curl_global_init(CURL_GLOBAL_ALL);

    curl = curl_easy_init();
    if (curl) {
      curl_easy_setopt(curl, CURLOPT_URL, this->mMasterServerAddress.c_str());
      // TODO: update with proper data
      std::string postData = "param1=value1&param2=value2";
      curl_easy_setopt(
        curl, CURLOPT_POSTFIELDS, "name=Server&project=Bomberbloke");

      /* Perform the request, res will get the return code */
      res = curl_easy_perform(curl);
      /* Check for errors */
      if (res != CURLE_OK) {
        fprintf(stderr,
                "\ncurl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
      } else {
        printf("\nSuccessfully updated game server at: %s\n",
               this->mMasterServerAddress.c_str());
      }
      curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
  } else
    return;
}

void
NetServer::syncPlayers()
{
  for (auto i = _player_list.begin(); i != _player_list.end(); i++) {
    int id = (*i)->getId();
    std::shared_ptr<AbstractEvent> s_event(new syncEvent(id));
    std::shared_ptr<GamePlayerProperties> p_props =
      (*i)->getPlayerProperties();
    if (p_props) {
      GamePlayerProperties props = *p_props;
      std::shared_ptr<AbstractEvent> p_event(
        new PlayerPropertiesEvent(props));
      //sendEvent(s_event, id); TODO This was duplicated, can it be removed?
      mConnector.sendEvent(p_event, id);
    } else {
      log_message(ERR, "Failed to sync properties - null properties");
    }
    mConnector.sendEvent(s_event, id);
    syncPlayerProperties(*i);
  }
  return;
}

void
NetServer::syncPlayerProperties(std::shared_ptr<AbstractPlayer> player)
{
  GamePlayerProperties g_props = *player->getPlayerProperties();
  std::shared_ptr<AbstractEvent> p_event(new PlayerPropertiesEvent(g_props));
  mConnector.sendEvent(p_event, player->getId());
}

void
NetServer::update()
{
  auto receivedEvents = mConnector.poll();
  for(auto pair : receivedEvents) {
    int id = pair.first;
    std::shared_ptr<AbstractEvent> event = pair.second;
    std::shared_ptr<AbstractPlayer> player = findPlayer(id);

    if(player == nullptr) {
      log_message(ERR, "Recieved event, but associated id could not be matched "
                       "to player");
      continue;
    }

    switch(event->getType()) {
      case EVENT_PLAYERLEAVE: {
        handlePlayerLeave(player);
        break;
      }
      default: {
        try {
          handleEvent(event, id);
        } catch (std::exception& e) {
          std::stringstream strstream;
          strstream << "Received malformed network event.\n" << event->getType();
          log_message(ERR, strstream.str());
        }
      }
    }
  }
  return;
}

void
NetServer::printPlayers()
{
  std::stringstream msg;
  msg << "there are " << _player_list.size() << " players connected\n\n";
  for (auto i = _player_list.begin(); i != _player_list.end(); i++) {
    msg << (*i)->getInfoString() << "\n";
  }
  log_message(INFO, msg.str());
}

void
NetServer::disconnectPlayer(std::shared_ptr<AbstractPlayer> p_player,
                            std::string reason)
{
  mConnector.disconnectPeer(p_player->getId(), reason);
  handlePlayerLeave(p_player);
  return;
}

void
NetServer::disconnectPlayer(const std::string& player_name, std::string reason)
{
  for(auto i = _player_list.begin(); i != _player_list.end(); i++) {
    if ((*i)->mNickname == player_name) {
      disconnectPlayer(*i, reason);
      return;
    }
  }
  log_message(
    ERR, "Unable to kick player, \"" + player_name + "\"; no matching players");
  return;
}

void
NetServer::handlePlayerLeave(const std::shared_ptr<AbstractPlayer>& p)
{
  if (!p)
    return;
  std::stringstream msg;
  msg << "Player \"" << p->getNickname() << "\" left the server!";
  log_message(INFO, msg.str());
  std::shared_ptr<AbstractEvent> m_event(new MessageEvent(msg.str()));
  mConnector.broadcastEvent(m_event);
  for (auto act = _pScene->mActors.begin(); act != _pScene->mActors.end();
       act++) {
    if ((*act)->getPlayerId() == p->getId())
      (*act)->remove();
  }
  _player_list.remove(p);
  return;
}

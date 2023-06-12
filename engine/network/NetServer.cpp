//
// Created by dave on 08.06.20.
//
#include "NetServer.hpp"
#include "CommandEvent.hpp"
#include "JoinEvent.hpp"
#include "KickEvent.hpp"
#include "MessageEvent.hpp"
#include "MetadataEvent.hpp"
#include "PlayerPropertiesEvent.hpp"
#include "QueryEvent.hpp"
#include "ServerInfoEvent.hpp"
#include "SyncEvent.hpp"
#include "acceptEvent.hpp"
#include "cereal/archives/portable_binary.hpp"
#include "engine.hpp"
#include "errorEvent.hpp"
#include <curl/curl.h>
#include <enet/enet.h>
#include <sstream>
#include <string>

NetServer::NetServer()
{
  // init_enet();
}

NetServer::~NetServer()
{
  // Clean up ENet
  stop();
  enet_deinitialize();
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
NetServer::handleJoinEvent(std::shared_ptr<JoinEvent> event, ENetPeer* from)
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
  auto player = std::make_shared<NetworkPlayer>(nickname, from);
  bool added = addPlayer(player);

  /* Send an acceptEvent */
  if (added) {
    std::unique_ptr<AbstractEvent> accept_event(new acceptEvent());
    sendEvent(accept_event, from);
    /*  Set the timeout for the new peer (in ms)*/
    enet_peer_timeout(from, 1000, 0, 0);
    /* Now sync send the entire gamestate to the client */
    std::unique_ptr<AbstractEvent> s_event(new SyncEvent());
    sendEvent(s_event, from);

    // Handle join commands e.g 'colour 0x0000ffff'
    for(auto& command : event->getCommands()){
      handleCommandEvent(std::make_shared<CommandEvent>(command), player);
    }
  }

  else {
    log_message(ERR, "Failed to add player");
  }
  return;
}

bool
NetServer::addPlayer(std::shared_ptr<AbstractPlayer> p_player)
{
  /* Finally check that there is no connected player with the same ENetAddress
   */
  ENetPeer* from = p_player->getPeer();
  for (auto i = _player_list.begin(); i != _player_list.end(); i++) {
    ENetPeer* tmp_peer = (*i)->getPeer();
    if (!tmp_peer)
      continue;
    if (tmp_peer == from) {
      log_message(INFO,
                  "a player at from " + std::to_string(from->address.host) +
                    ":" + std::to_string(from->address.port) +
                    "is already connected");
      std::unique_ptr<AbstractEvent> e_event(
        new errorEvent("Player from host already connected"));
      sendEvent(e_event, from);
      return false;
    }
  }

  for (unsigned int j = 1; j < BLOKE_MAX_ID; j++) {
    /* True if no actor has id j */
    bool set = true;
    for (auto i = _player_list.begin(); i != _player_list.end(); i++) {
      if ((*i)->getId() == j) {
        set = false;
        break;
      }
    }
    if (set) {
      p_player->setId(j);
      _player_list.push_back(p_player);
      return true;
    }
  }
  return false;
}

void
NetServer::handleEvent(std::shared_ptr<AbstractEvent> pEvent, ENetPeer* from)
{
  if (!pEvent.get()) {
    log_message(ERR, "tried to handle a null event");
    return;
  }

  std::shared_ptr<AbstractPlayer> p_player;

  for (auto i = _player_list.begin(); i != _player_list.end(); i++) {
    ENetAddress* players_address =
      ((*i)->getPeer()) ? &((*i)->getPeer()->address) : nullptr;
    if (!players_address)
      continue;
    if (players_address->host == from->address.host &&
        players_address->port == from->address.port) {
      p_player = *i;
      break;
    }
  }

  switch (pEvent->getType()) {
    case EVENT_QUERY: {
      /*  Get the server info and see if the username may be used */
      std::shared_ptr<QueryEvent> pquery_event =
        std::dynamic_pointer_cast<QueryEvent>(pEvent);
      std::unique_ptr<AbstractEvent> info_event(new ServerInfoEvent(
        mServerInfo, _player_list, pquery_event->getNickname()));
      sendEvent(info_event, from);
      break;
    }

    case EVENT_JOIN: {
      std::shared_ptr<JoinEvent> pjoin_event =
        std::dynamic_pointer_cast<JoinEvent>(pEvent);
      handleJoinEvent(pjoin_event, from);
      break;
    }

    case EVENT_COMMAND: {
      std::shared_ptr<CommandEvent> p_command_event =
        std::dynamic_pointer_cast<CommandEvent>(pEvent);
      handleCommandEvent(p_command_event, p_player);
      break;
    }
    default:
      break;
  }

  return;
}

void NetServer::handleCommandEvent(std::shared_ptr<CommandEvent> c_event, std::shared_ptr<AbstractPlayer> p_player){
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

void
NetServer::poll()
{
  // wait half a second second before processing all requests
  ENetEvent event;
  while (enet_host_service(mENetServer, &event, 0) > 0) {
    switch (event.type) {
      case ENET_EVENT_TYPE_RECEIVE: {
        if (std::strcmp(reinterpret_cast<const char*>(event.packet->data),
                        "ping") == 0) {
          log_message(DEBUG, "Recieved ping command from peer");

          ENetPacket* pong_packet = enet_packet_create(
            "pong", strlen("pong") + 1, ENET_PACKET_FLAG_RELIABLE);
          sendPacket(event.peer, pong_packet, 0);
          log_message(0, "Sent Packet to Peer");
          enet_host_flush(mENetServer);
        }
        std::stringstream data_in;
        data_in.write((char*)event.packet->data, event.packet->dataLength);

        try {
          std::unique_ptr<AbstractEvent> receive_event;
          cereal::PortableBinaryInputArchive inArchive(data_in);
          inArchive(receive_event);
          std::shared_ptr<AbstractEvent> sp_to_handle(std::move(receive_event));

          /* Make the pointer shared so we can handle it elsewhere */
          handleEvent(sp_to_handle, event.peer);
        } catch (std::exception& e) {
          std::stringstream strstream;
          strstream << "Received malformed network event.\n" << e.what();
          log_message(ERR, strstream.str());
        }

        /* Clean up the packet now that we're done using it. */
        enet_packet_destroy(event.packet);

        break;
      }
      case ENET_EVENT_TYPE_CONNECT: {
        log_message(0, "User made ENet connection");
        std::stringstream message;
        break;
      }

      case ENET_EVENT_TYPE_DISCONNECT: {
        log_message(INFO, "Disconnect event received");
        /* Reset the peer's client information. */
        event.peer->data = NULL;
        /* Remove peer/player from all lists */
        while (true) {
          auto p_iter = std::find_if(
            _player_list.begin(),
            _player_list.end(),
            [&](std::shared_ptr<AbstractPlayer> p) -> bool {
              return (p->getPeer() != nullptr) && (p->getPeer() == event.peer);
            });
          if (p_iter != _player_list.end())
            handlePlayerLeave(*p_iter);
          else
            break;
        }
      }
      default:
        break;
    }
  }
  return;
}

//! Initialise ENet, and create server instance.
// Returns: false if failed to create server
bool
NetServer::init_enet()
{
  if (enet_initialize() != 0) {
    printf("FATAL: Could not init enet\n");
    return 0;
  }
  mENetAddress.host = ENET_HOST_ANY; // 0
  mENetAddress.port = mPort;

  // Config for defaults can be defined later
  // const ENetAddress *address, size_t peerCount, size_t channelLimit,
  // enet_uint32 incomingBandwidth, enet_uint32 outgoingBandwidth)
  mENetServer = enet_host_create(&mENetAddress, 124, 10, 0, 0);

  if (mENetServer == NULL) {
    log_message(ERR, "Could not start server. Is the port already in use?");
    return 0;
  } else {
      log_message(INFO,"Listening on port: " + std::to_string(mPort));
    return 1;
  }
}

bool
NetServer::stop()
{
  // TODO: Disconnect clients gracefully
  updateGameMasterServer(true);
  return 0;
}

void
NetServer::setPort(short port)
{
  mPort = port;
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
NetServer::sendStringMessage(std::string message, ENetPeer* to)
{
  ENetPacket* packet = enet_packet_create(
    message.c_str(), strlen(message.c_str()) + 1, ENET_PACKET_FLAG_RELIABLE);
  enet_peer_send(to, 0, packet);

  log_message(DEBUG,
              "Sent Message" + message + "to " +
                std::to_string(to->address.host) + ":" +
                std::to_string(to->address.port));

  return;
}

void
NetServer::sendEvent(std::unique_ptr<AbstractEvent>& event, ENetPeer* to)
{
  std::stringstream blob;
  {
    cereal::PortableBinaryOutputArchive oArchive(blob);
    oArchive(event);
  }

  // log_message(DEBUG, "Sending" + blob.str());
  bool reliable = false;
  switch (event->getType()) {
    case EVENT_SYNC:
    case EVENT_INFO:
    case EVENT_CREATE:
    case EVENT_REMOVE:
    case EVENT_QUERY:
    case EVENT_MESSAGE:
    case EVENT_COMMAND:
    case EVENT_PROPERTIES:
    case EVENT_KICK:
    case EVENT_ACCEPT:
    case EVENT_ERROR:
      reliable = true;
    default:
      break;
  }

  ENetPacket* packet;
  std::string message = blob.str();
  if (reliable)
    packet = enet_packet_create(
      message.c_str(), message.size(), ENET_PACKET_FLAG_RELIABLE);
  else
    packet = enet_packet_create(message.c_str(), message.size(), 0);
  enet_peer_send(to, 0, packet);

  return;
}

void
NetServer::flush()
{
  enet_host_flush(mENetServer);
}

void
NetServer::broadcastEvent(std::unique_ptr<AbstractEvent>& event)
{
  for (auto i = _player_list.begin(); i != _player_list.end(); i++) {
    if ((*i)->getPeer())
      sendEvent(event, (*i)->getPeer());
  }
}

void
NetServer::syncPlayers()
{
  // Each player receives a SyncEvent, PlayerPropertiesEvent and MetadataEvent
  auto *metadata_event = new MetadataEvent();

  for (auto i = _player_list.begin(); i != _player_list.end(); i++) {
    ENetPeer* peer = (*i)->getPeer();
    if (peer) {
      std::unique_ptr<AbstractEvent> s_event(new SyncEvent(peer));
      std::shared_ptr<GamePlayerProperties> p_props =
        (*i)->getPlayerProperties();

      // Use this opportunity to update ping
      int ping = (int) peer->lastRoundTripTime;
      std::string key = "lastPingMeasurement";
      (*i)->mMetadata.numeric[key] = ping;
      metadata_event->includeUpdateNumeric((int) (*i)->getId(), key, ping);

      if (p_props) {
        GamePlayerProperties props = *p_props;
        std::unique_ptr<AbstractEvent> p_event(
          new PlayerPropertiesEvent(props));
        sendEvent(s_event, peer);
        sendEvent(p_event, peer);
      } else {
        log_message(ERR, "Failed to sync properties - null properties");
      }
      sendEvent(s_event, peer);
    }
    syncPlayerProperties(*i);
  }

  // Finally send everyone new metadata
  std::unique_ptr<AbstractEvent> e(metadata_event);
  broadcastEvent(e);
  return;
}

void
NetServer::syncPlayerProperties(std::shared_ptr<AbstractPlayer> player)
{
  ENetPeer* peer = player->getPeer();

  if (!peer)
    return;
  GamePlayerProperties g_props = *player->getPlayerProperties();
  std::unique_ptr<AbstractEvent> p_event(new PlayerPropertiesEvent(g_props));
  sendEvent(p_event, peer);
}

void
NetServer::broadcastPacket(ENetPacket* packet, enet_uint8 channel)
{
  if (clientCount() == 0) {
    log_message(ERR, "No Clients Connected, Could not Broadcast Message!");
  } else {
    enet_host_broadcast(mENetServer, channel, packet);
    enet_host_flush(mENetServer);
  }
}

void
NetServer::sendPacket(ENetPeer* peer, ENetPacket* packet, enet_uint8 channel)
{
  enet_peer_send(peer, channel, packet);
  enet_host_flush(mENetServer);
}

void
NetServer::disconnectPeer(ENetPeer* peer)
{
  // forcefully disconnect peer
  enet_peer_reset(peer);
}

bool
NetServer::status()
{
  // True if server is still active
  return mENetServer != nullptr;
}

uint32_t
NetServer::clientCount() const
{
  return mENetServer->connectedPeers;
}

enet_uint32
NetServer::packetsSent() const
{
  return mENetServer->totalSentPackets;
}

enet_uint32
NetServer::packetsRecieved() const
{
  return mENetServer->totalReceivedPackets;
}

void
NetServer::update()
{
  flush();
  poll();
  return;
}

void
NetServer::printPlayers()
{
  /*  TODO include ping*/
  std::stringstream msg;
  msg << "there are " << _player_list.size() << " players connected\n\n";
  for (auto i = _player_list.begin(); i != _player_list.end(); i++) {
    msg << (*i)->getInfoString() << "\n";
  }
  log_message(INFO, msg.str());
}

void
NetServer::disconnectPlayer(std::shared_ptr<AbstractPlayer> p_player,
                            std::string reason,
                            bool pause)
{
  ENetPeer* peer = p_player->getPeer();
  if (peer) {
    /*First send a kick event*/
    std::unique_ptr<AbstractEvent> k_event(new KickEvent(reason));
    sendEvent(k_event, peer);
    if (pause)
      SDL_Delay(500);
    /* Now disconnect the peer */
    enet_peer_disconnect(peer, 0);
    SDL_Delay(1000);
    enet_peer_reset(peer);
  }
  handlePlayerLeave(p_player);
  return;
}

void
NetServer::disconnectPlayer(std::string player_name, std::string reason)
{
  for (auto i = _player_list.begin(); i != _player_list.end(); i++) {
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
NetServer::handlePlayerLeave(std::shared_ptr<AbstractPlayer> p)
{
  if (!p)
    return;
  std::stringstream msg;
  msg << "Player \"" << p->getNickname() << "\" left the server!";
  log_message(INFO, msg.str());
  std::unique_ptr<AbstractEvent> m_event(new MessageEvent(msg.str()));
  broadcastEvent(m_event);
  for (auto act = _pScene->mActors.begin(); act != _pScene->mActors.end();
       act++) {
    if ((*act)->getPlayerId() == p->getId())
      (*act)->remove();
  }

  _player_list.remove(p);
  return;
}

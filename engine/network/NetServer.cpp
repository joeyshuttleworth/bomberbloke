//
// Created by dave on 08.06.20.
//
#include "engine.hpp"
#include "NetServer.hpp"
#include "CommandEvent.hpp"
#include "QueryEvent.hpp"
#include "ServerInfoEvent.hpp"
#include "JoinEvent.hpp"
#include "acceptEvent.hpp"
#include "errorEvent.hpp"
#include "syncEvent.hpp"
#include "engine.hpp"
#include <enet/enet.h>
#include <curl/curl.h>
#include <string>
#include <sstream>
#include "cereal/archives/json.hpp"

NetServer::NetServer() {
    // init_enet();
}

NetServer::~NetServer() {
    // Clean up ENet
    stop();
    enet_deinitialize();
}

void NetServer::pollLoop(){
  while(!_halt){
    poll();
  }
  return;
}

void NetServer::handleJoinEvent(std::shared_ptr<JoinEvent> event, ENetPeer *from){

  std::string nickname = event->mNickname;

  /*  First check that there's space on the server */
  if(_server_info.mMaxPlayers <= _player_list.size()){
    return;
  }

  /* Second check that no player has the same nickname */
  auto iter = find_if(_player_list.begin(), _player_list.end(), [&](std::shared_ptr<AbstractPlayer> p){return nickname == p->mNickname;});

  if(iter != _player_list.end()){
    return;
  }

  /* Finally check that there is no connected player with the same ENetAddress */
  for(auto i = _player_list.begin(); i != _player_list.end(); i++){
    ENetPeer*tmp_peer = (*i)->getPeer();

    if(!tmp_peer)
      continue;

    if(tmp_peer == from){
      log_message(INFO, "a player at from " + std::to_string(from->address.host) + ":" + std::to_string(from->address.port) + "is already connected");
      std::unique_ptr<AbstractEvent> e_event(new errorEvent("Player from host already connected"));
      sendEvent(e_event, from);
    }
  }

  /* Send an acceptEvent */
  {
    std::unique_ptr<AbstractEvent> accept_event(new acceptEvent());
    sendEvent(accept_event, from);
  }

  /* Add the player to _player_list */
  _player_list.push_back(std::shared_ptr<NetworkPlayer>(new NetworkPlayer(nickname, from)));

  /* Remove player from mUnjoinedPeers */
  mUnjoinedPeers.remove_if([&](unjoinedPeer up) -> bool{return up.peer == from;});


  /* Now sync send the entire gamestate to the client */
  std::unique_ptr<AbstractEvent> s_event(new syncEvent());
  sendEvent(s_event, from);
  return;
}

void NetServer::handleEvent(std::shared_ptr<AbstractEvent> pEvent, ENetPeer *from){
  if(!pEvent.get()){
    log_message(ERROR, "tried to handle a null event");
    return;
  }

  std::shared_ptr<AbstractPlayer> p_player;

  for(auto i = _player_list.begin(); i != _player_list.end(); i++){
    ENetAddress *players_address = ((*i)->getPeer()) ? &((*i)->getPeer()->address) : nullptr;
    if(!players_address)
      continue;
    if(players_address->host == from->address.host && players_address->port == from->address.port){
      p_player = *i;
      break;
    }
  }

  switch(pEvent->getType()){
  case EVENT_QUERY:{
    /*  Get the server info and see if the username may be used */
    std::shared_ptr<QueryEvent> pquery_event = std::dynamic_pointer_cast<QueryEvent>(pEvent);
    std::unique_ptr<AbstractEvent> info_event(new ServerInfoEvent(mServerInfo, _player_list, pquery_event->getNickname()));
    sendEvent(info_event, from);
    break;
  }

  case EVENT_JOIN:{
      std::shared_ptr<JoinEvent> pjoin_event = std::dynamic_pointer_cast<JoinEvent>(pEvent);
      handleJoinEvent(pjoin_event, from);
      break;
  }

  case EVENT_COMMAND:{
    std::shared_ptr<CommandEvent> c_event =  std::dynamic_pointer_cast<CommandEvent>(pEvent);
    if(!p_player){
      log_message(INFO, "Command event received from non-connected player");
      return;
    }
    auto character = p_player->getCharacter();
    if(!character){
      log_message(INFO, "Command received from player without character");
      return;
    }
    std::string command = c_event->getCommand();
    log_message(DEBUG, "received command " + command);
    if(command != ""){
      character->handle_command(command);
    }
    break;
  }
  default:
    break;
  }

  return;
}

void NetServer::poll() {
  // wait half a second second before processing all requests
  ENetEvent event;
  while (enet_host_service(mENetServer, &event, 0) > 0) {
    switch (event.type) {
    case ENET_EVENT_TYPE_RECEIVE: {
      if (std::strcmp(reinterpret_cast<const char *>(event.packet->data), "ping") == 0) {
        log_message(DEBUG,"Recieved ping command from peer");

        ENetPacket *pong_packet = enet_packet_create("pong",
                                                     strlen("pong") + 1,
                                                     ENET_PACKET_FLAG_RELIABLE);
        sendPacket(event.peer, pong_packet, 0);
        log_message(0,"Sent Packet to Peer");
        enet_host_flush(mENetServer);
      }
      if (std::strcmp(reinterpret_cast<const char *>(event.packet->data), "ping_all") == 0) {
        log_message(0,"Recieved ping all command");
        SDL_Delay(500);
        ENetPacket *pong_packet = enet_packet_create("pong_all!",
                                                     strlen("pong_all!") + 1,
                                                     ENET_PACKET_FLAG_RELIABLE);
        broadcastPacket(pong_packet, 0);
        log_message(0,"Sent Packet to ALL");
        enet_host_flush(mENetServer);
        SDL_Delay(500);

        broadcastPacket(pong_packet, 0);
        log_message(0,"Sent Packet to ALL");
        enet_host_flush(mENetServer);
        SDL_Delay(500);
        broadcastPacket(pong_packet, 0);
        log_message(0,"Sent Packet to ALL");
        enet_host_flush(mENetServer);
      }
      log_message(0, "Recieved packet");

      std::stringstream message;
      message << "A packet of length " << event.packet->dataLength << " was received from '";
      message << event.peer->data << "' on channel: " << event.channelID;
      log_message(0, message.str());

      std::stringstream packet_message;
      packet_message << "Packet message: '" << event.packet->data << "'";
      log_message(0, packet_message.str());

      // handle packet here enet_uint32

      std::stringstream data_in;
      data_in << event.packet->data;

      log_message(DEBUG, "data received " + data_in.str());

      std::unique_ptr<AbstractEvent> receive_event;
      {
        cereal::JSONInputArchive inArchive(data_in);
        inArchive(receive_event);
      }

      std::shared_ptr<AbstractEvent> sp_to_handle(std::move(receive_event));

      /* Make the pointer shared so we can handle it elsewhere */
      handleEvent(sp_to_handle, event.peer);

      /* Clean up the packet now that we're done using it. */
      enet_packet_destroy(event.packet);

      break;
    }
    case ENET_EVENT_TYPE_CONNECT: {
      log_message(0, "User Connected");
      std::stringstream message;
      //message << "A packet of length " << event.packet->dataLength << " containing ";
      //              message << event.packet->data << " was received from " << event.peer->data;
      message << " on channel " << event.channelID << ".\n";
      std::cout << message.str();

      unjoinedPeer u_peer;
      u_peer.peer = event.peer;
      u_peer.timeout = 600;

      mUnjoinedPeers.push_back(u_peer);

      break;
    }

    case ENET_EVENT_TYPE_DISCONNECT:
      // Reset the peer's client information.
      event.peer->data = NULL;
    default:
      break;
    }
  }
}

//! Initialise ENet, and create server instance.
// Returns: false if failed to create server
bool NetServer::init_enet() {
    if (enet_initialize() != 0) {
        printf("FATAL: Could not init enet\n");
        return 0;
    }
    mENetAddress.host = ENET_HOST_ANY; //0
    mENetAddress.port = mPort;

    //Config for defaults can be defined later
    //const ENetAddress *address, size_t peerCount, size_t channelLimit, enet_uint32 incomingBandwidth, enet_uint32 outgoingBandwidth)
    mENetServer = enet_host_create(&mENetAddress, 100, 2, 0, 0);

    if (mENetServer == NULL) {
      log_message(ERROR, "Could not start server. Is the port already in use?");
      return 0;
    }
    else
      return 1;
}

bool NetServer::stop() {
    //TODO: Disconnect clients gracefully
    updateGameMasterServer(true);
    return 0;
}

void NetServer::removeFromMasterServer() {
    updateGameMasterServer(true);
    // printf("%s disconnected.\n", event.peer->data);
}

void NetServer::updateGameMasterServer(bool disconnect) {
    CURL *curl;
    CURLcode res;
    if (!disconnect) {
        /* In windows, this will init the winsock stuff */
        curl_global_init(CURL_GLOBAL_ALL);

        curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, this->masterServerAddress.c_str());
            //TODO: update with proper data
            std::string postData = "param1=value1&param2=value2";
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "name=Server&project=Bomberbloke");

            /* Perform the request, res will get the return code */
            res = curl_easy_perform(curl);
            /* Check for errors */
            if (res != CURLE_OK) {
                fprintf(stderr, "\ncurl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            } else {
                printf("\nSuccessfully updated game server at: %s\n", this->masterServerAddress.c_str());
            }
            curl_easy_cleanup(curl);
        }
        curl_global_cleanup();
    } else
        return;
}

void NetServer::sendStringMessage(std::string message, ENetPeer *to){
  ENetPacket *packet = enet_packet_create(message.c_str(), strlen(message.c_str())+1, ENET_PACKET_FLAG_RELIABLE);
  enet_peer_send(to, 0, packet);

  enet_host_flush(mENetServer);

  log_message(DEBUG,  "Sent Message" + message + "to " + std::to_string(to->address.host) + ":" + std::to_string(to->address.port));

  return;
}

void NetServer::sendEvent(std::unique_ptr<AbstractEvent> &event, ENetPeer *to){
  std::stringstream blob;
  {
    cereal::JSONOutputArchive oArchive(blob);
    oArchive(event);
  }

  sendStringMessage(blob.str(), to);

  log_message(DEBUG, "Sending" + blob.str());
  return;
}

void NetServer::broadcastEvent(std::unique_ptr<AbstractEvent>& event){
  for(auto i = _player_list.begin(); i != _player_list.end(); i++){
    if((*i)->getPeer())
      sendEvent(event, (*i)->getPeer());
  }
}

void NetServer::syncPlayers(){
  for(auto i = _player_list.begin(); i != _player_list.end(); i++){
    ENetPeer *peer = (*i)->getPeer();
    if(peer){
      std::unique_ptr<AbstractEvent> s_event(new syncEvent(peer));
      sendEvent(s_event, peer);
    }
  }
  return;
}


void NetServer::broadcastPacket(ENetPacket *packet, enet_uint8 channel) {
    if (clientCount() == 0) {
        log_message(0, "ERROR: No Clients Connected, Could not Broadcast Message!");
    }
    else {
        enet_host_broadcast(mENetServer, channel, packet);
        enet_host_flush(mENetServer);
    }
}

void NetServer::sendPacket(ENetPeer *peer, ENetPacket *packet, enet_uint8 channel) {
    enet_peer_send(peer, channel, packet);
    enet_host_flush(mENetServer);
}


void NetServer::disconnectPeer(ENetPeer *peer) {
    // forcefully disconnect peer
    enet_peer_reset(peer);
}

bool NetServer::status() {
    // True if server is still active
    return mENetServer != nullptr;
}

uint32_t NetServer::clientCount() const {
    return mENetServer->connectedPeers;
}

enet_uint32 NetServer::packetsSent() const {
    return mENetServer->totalSentPackets;
}

enet_uint32 NetServer::packetsRecieved() const {
    return mENetServer->totalReceivedPackets;
}

void NetServer::update(){
  if(_tick%60 == 0){
    for(auto i = mUnjoinedPeers.begin(); i != mUnjoinedPeers.end(); i++){
      if(i->timeout > 60){
        i->timeout -= 60;
      }
      else{
        i->timeout = 0;
      }
    }
    /* Removed all timed out peers */
    mUnjoinedPeers.remove_if([](unjoinedPeer up) -> bool{return up.timeout==0;});
  }
  return;
}



//
// Created by dave on 08.06.20.
//

#include "NetServer.h"
#include "engine.h"
#include <enet/enet.h>
#include <curl/curl.h>
#include <string>
#include <sstream>
Server::Server() {
    init_enet();
}

Server::~Server() {
    // Clean up ENet
    stop();
    enet_deinitialize();
}

//! Poll and Handle events from ENet, by default will wait one second for an event before processing
void Server::poll() {
    // wait one second for event

  while (enet_host_service(this->server, &event, 1000) > 0) {
        switch (event.type) {
        case ENET_EVENT_TYPE_CONNECT:{
              std::stringstream message;
              message <<  "A packet of length " << event.packet->dataLength << " containing ";
              message << event.packet->data << " was received from " << event.peer->data;
              message << " on channel " << event.channelID << ".\n";
              log_message(INFO, message.str());
              // TODO: Add user to user_list
                break;
        }
        case ENET_EVENT_TYPE_RECEIVE:{
              std::stringstream message;
              message << "A packet of length " << event.packet->dataLength << " was received from ";
              message << event.peer->data << "on channel " << event.channelID;
              log_message(INFO, message.str());
                /* Clean up the packet now that we're done using it. */
                enet_packet_destroy(event.packet);
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
bool Server::init_enet() {
    if (enet_initialize() != 0) {
        printf("FATAL: Could not init enet\n");
        return 0;
    }
    this->address.host = ENET_HOST_ANY; //0
    this->address.port = this->PORT;

    //Config for defaults can be defined later
    //const ENetAddress *address, size_t peerCount, size_t channelLimit, enet_uint32 incomingBandwidth, enet_uint32 outgoingBandwidth)
    this->server = enet_host_create(&this->address, 100, 2, 0, 0);

    if (this->server == NULL) {
        printf("FATAL: Could not init server at port %i, is it already in use?\n", PORT);
        return 0;
    } else { return 1; };
}

bool Server::stop() {
    //TODO: Disconnect clients gracefully
    return 0;
}

void Server::removeFromMasterServer(){
  updateGameMasterServer(true);
  // printf("%s disconnected.\n", event.peer->data);
}


void Server::updateGameMasterServer(bool disconnect)
{
    CURL *curl;
    CURLcode res;
    if(!disconnect){
      /* In windows, this will init the winsock stuff */
      curl_global_init(CURL_GLOBAL_ALL);
      curl = curl_easy_init();
      if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, this->mMasterServerAddress.c_str());
        std::string postData =  _server_info.getInfo();
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if(res != CURLE_OK)
          fprintf(stderr, "curl_easy_perform() failed: %s\n",
                  curl_easy_strerror(res));

        curl_easy_cleanup(curl);
      }
      curl_global_cleanup();

    }
    else
        return;
}

void Server::broadcastPacket(ENetPacket *packet, enet_uint8 channel) {
    enet_host_broadcast(this->server, channel, packet);

}

void Server::sendPacket(ENetPeer *peer, ENetPacket *packet, enet_uint8 channel) {
    enet_peer_send(peer, channel, packet);
}


void Server::disconnectPeer(ENetPeer *peer) {
    // forcefully disconnect peer
    enet_peer_reset(peer);
}

bool Server::status() {
    // True if server is still active
    return this->server != nullptr;
}

uint32_t Server::clientCount() const {
    return this->server->connectedPeers;
}

enet_uint32 Server::packetsSent() const {
    return this->server->totalSentPackets;
}

enet_uint32 Server::packetsRecieved() const {
    return this->server->totalReceivedPackets;
}



//
// Created by dave on 08.06.20.
//

#include "Server.h"
#include <enet/enet.h>
#include <stdio.h>
#include <curl/curl.h>

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
            case ENET_EVENT_TYPE_CONNECT:
                printf("A new client connected from %x:%u.\n",
                       event.peer->address.host,
                       event.peer->address.port);
                // TODO: Add user to user_list
                break;
            case ENET_EVENT_TYPE_RECEIVE:
                printf("A packet of length %u containing %s was received from %s on channel %u.\n",
                       event.packet->dataLength,
                       event.packet->data,
                       event.peer->data,
                       event.channelID);
                /* Clean up the packet now that we're done using it. */
                enet_packet_destroy(event.packet);

                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                printf("%s disconnected.\n", event.peer->data);
                // Reset the peer's client information.
                event.peer->data = NULL;
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
    updateGameMasterServer(true)
}


void Server::updateGameMasterServer(bool disconnect)
{
    CURL *curl;
    CURLcode res;

    /* In windows, this will init the winsock stuff */
    curl_global_init(CURL_GLOBAL_ALL);
    
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, this->masterServerAddress);
        std::string postData = this->getCurrentServerInfo;
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



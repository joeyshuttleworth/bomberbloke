#include <ENetConnector.hpp>
#include <enet/enet.h>
#include "cereal/archives/portable_binary.hpp"
#include <KickEvent.hpp>
#include <PlayerLeaveEvent.hpp>
#include <engine.hpp>

ENetConnector::ENetConnector()
{
  if (enet_initialize() != 0) {
    printf("ERROR: An error occurred while initializing ENet");
    return;
  }
  mENetHost = enet_host_create(nullptr, 1, 1, 0, 0);
}

ENetConnector::ENetConnector(int port) : Connector(port)
{
  if (enet_initialize() != 0) {
    printf("FATAL: Could not init enet\n");
    return;
  }
  mENetAddress.host = ENET_HOST_ANY; // 0
  mENetAddress.port = port;

  // Config for defaults can be defined later
  // const ENetAddress *address, size_t peerCount, size_t channelLimit,
  // enet_uint32 incomingBandwidth, enet_uint32 outgoingBandwidth)
  mENetHost = enet_host_create(&mENetAddress, 124, 10, 0, 0);
  if (mENetHost == NULL) {
    log_message(ERR, "Could not start server. Is the port already in use?");
  } else {
    log_message(INFO,"Listening on port: " + std::to_string(port));
  }
}

ENetConnector::~ENetConnector() {
  if(mENetHost != nullptr)
    enet_host_destroy(mENetHost);
  enet_deinitialize();
}

void
ENetConnector::sendPacket(ENetPeer* peer, ENetPacket* packet, enet_uint8 channel)
{
  enet_peer_send(peer, channel, packet);
  enet_host_flush(mENetHost);
}

void
ENetConnector::sendEvent(std::shared_ptr<AbstractEvent> event, int to_id) {
  if(peers.count(to_id) == 0)
    return;

  std::shared_ptr<ENetPeer> to = peers[to_id];
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
  enet_peer_send(to.get(), 0, packet);

  return;
}

void
ENetConnector::disconnectPeer(int id, std::string reason) {
  if(!peers.count(id))
    return;

  std::shared_ptr<ENetPeer> player = peers[id];
  ENetPeer *peer = player.get();

  /* First send a kick event */
  std::shared_ptr<AbstractEvent> k_event(new KickEvent(reason));
  sendEvent(k_event, id);

  /* Now disconnect the peer */
  enet_peer_disconnect(peer, 0);
  enet_peer_reset(peer);

  return;
}

std::list<std::pair<int, std::shared_ptr<AbstractEvent>>>
ENetConnector::poll() {
  enet_host_flush(mENetHost); // TODO figure exactly what this does?

  // Poll for events and return as { peer_id, shared_ptr<AbstractEvent> }
  std::list<std::pair<int, std::shared_ptr<AbstractEvent>>> events;

  ENetEvent event;
  while (enet_host_service(mENetHost, &event, 0) > 0) {
    int id = findIdFromPeer(event.peer);
    if(id == -1) {
      id = nextFreeId();
      peers[id] = std::make_shared<ENetPeer>(*event.peer);
      /*  Set the timeout for the new peer (in ms)*/
      enet_peer_timeout(event.peer, 1000, 0, 0);
    }
    switch (event.type) {
      case ENET_EVENT_TYPE_RECEIVE: {
        if (std::strcmp(reinterpret_cast<const char*>(event.packet->data),
                        "ping") == 0) {
          log_message(DEBUG, "Recieved ping command from peer");

          ENetPacket* pong_packet = enet_packet_create(
            "pong", strlen("pong") + 1, ENET_PACKET_FLAG_RELIABLE);
          sendPacket(event.peer, pong_packet, 0);
          log_message(0, "Sent Packet to Peer");
          enet_host_flush(mENetHost);
          continue;
        }
        std::stringstream data_in;
        data_in.write((char*)event.packet->data, event.packet->dataLength);

        try {
          std::unique_ptr<AbstractEvent> receive_event;
          cereal::PortableBinaryInputArchive inArchive(data_in);
          inArchive(receive_event);

          std::shared_ptr<AbstractEvent> sp_to_handle(std::move(receive_event));
          events.emplace_back(id, sp_to_handle);
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
        peers.erase(id);

        std::shared_ptr<AbstractEvent> sp_to_handle = std::make_shared<PlayerLeaveEvent>(id);
        events.emplace_back(id, sp_to_handle);
      }
      default:
        break;
    }
  }

  return events;
}

#include "ENetConnector.hpp"
#include <enet/enet.h>
#include "cereal/archives/portable_binary.hpp"
#include <KickEvent.hpp>
#include <PlayerLeaveEvent.hpp>
#include <engine.hpp>

ENetConnector::ENetConnector()
{
  printf("ENetConnector() called \n");
}

void
ENetConnector::configure(ushort port)
{
  mPort = port;
  mENetAddress.host = ENET_HOST_ANY;
  mENetAddress.port = port;
  mAddressConfigured = true;
}

void
ENetConnector::open()
{
  if (enet_initialize() != 0) {
    printf("FATAL: Could not init enet\n");
    return;
  }
  // Config for defaults can be defined later
  // const ENetAddress *address, size_t peerCount, size_t channelLimit,
  // enet_uint32 incomingBandwidth, enet_uint32 outgoingBandwidth)
  if(mAddressConfigured)
    mENetHost = enet_host_create(&mENetAddress, 124, 10, 0, 0);
  else
    mENetHost = enet_host_create(nullptr, 1, 1, 0, 0);

  if (mENetHost == NULL) {
    log_message(ERR, "Could not start server. Is the port already in use?");
  } else {
    log_message(INFO,"Listening on port: " + std::to_string(mPort));
  }
}

void
ENetConnector::close()
{
  printf("ENetConnector : closing on port %i\n", mPort);
  peers.clear();
  if(mENetHost != nullptr)
    enet_host_destroy(mENetHost);
  enet_deinitialize();
}

ENetConnector::~ENetConnector() {
  printf("ENetConnector destructor called\n");
}

void
ENetConnector::sendPacket(ENetPeer* peer, ENetPacket* packet, enet_uint8 channel)
{
  enet_peer_send(peer, channel, packet);
  enet_host_flush(mENetHost);
}

void
ENetConnector::sendEvent(AbstractEvent &event, int to_id)
{
  if(mENetHost == nullptr) {
    fprintf(stderr, "Need to call configure() and open() on connector before"
                    " opening connections\n");
    return;
  }

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
  switch (event.getType()) {
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

}

int
ENetConnector::connectPeer(std::string address, short port)
{
  // Note for ENet, this function blocks the current thread until a
  // ENET_EVENT_TYPE_CONNECT confirms that a connection has been established.

  if(mENetHost == nullptr) {
    fprintf(stderr, "Need to call configure() and open() on connector before"
                    " opening connections\n");
    return -1;
  }

  ENetAddress addr;
  enet_address_set_host(&addr, address.c_str());
  addr.port = port;

  int newId = nextFreeId();
  peers[newId] = std::make_shared<ENetPeer>(*enet_host_connect(mENetHost,
                                                               &addr, 1, 0));

  ENetEvent event;
  if (enet_host_service(mENetHost, &event, 5000) > 0 &&
      event.type == ENET_EVENT_TYPE_CONNECT) {
    std::stringstream msg;
    printf("ENet: Connection to %s : %i succeeded\n", address.c_str(), port);
    return newId;
  } else {
    printf("ENet: Connection to %s : %i failed\n", address.c_str(), port);
    return -1;
  }
}

void
ENetConnector::disconnectPeer(int id, std::string reason) {
  // TODO This shouldn't send a kick event, so it can be used by client
  if(!peers.count(id))
    return;

  std::shared_ptr<ENetPeer> player = peers[id];
  ENetPeer *peer = player.get();

  /* First send a kick event */
  std::unique_ptr<AbstractEvent> k_event(new KickEvent(reason));
  sendEvent(k_event, id);

  /* Now disconnect the peer */
  enet_peer_disconnect(peer, 0);
  enet_peer_reset(peer);

  return;
}


std::list<std::pair<int, std::shared_ptr<AbstractEvent>>>
ENetConnector::poll(int timeout) {
  if(mENetHost == nullptr) {
    fprintf(stderr, "Need to call configure() and open() on connector before"
                    " opening connections\n");
  }

  enet_host_flush(mENetHost); // TODO figure exactly what this does?

  // Poll for events and return as { peer_id, shared_ptr<AbstractEvent> }
  std::list<std::pair<int, std::shared_ptr<AbstractEvent>>> events;

  ENetEvent event;
  while (enet_host_service(mENetHost, &event, (enet_uint32) timeout) > 0) {
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

        // TODO make this make sense from the clients perspective
        std::shared_ptr<AbstractEvent> sp_to_handle = std::make_shared<PlayerLeaveEvent>(id);
        events.emplace_back(id, sp_to_handle);
      }
      default:
        break;
    }
  }

  return events;
}

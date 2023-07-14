#include "ENetConnector.hpp"
#include <enet/enet.h>
#include "cereal/archives/portable_binary.hpp"
#include <KickEvent.hpp>
#include <PlayerLeaveEvent.hpp>
#include <engine.hpp>
#include <set>

ENetConnector::ENetConnector()
{
  //printf("ENetConnector() constructor called\n");
}

void
ENetConnector::configure(ushort port)
{
  // Sets connector to listen to a specific port, i.e. in 'server' mode.
  mPort = port;
  mENetAddress.host = ENET_HOST_ANY;
  mENetAddress.port = port;
  mAddressConfigured = true;
}

void
ENetConnector::open()
{
  log_message(DEBUG, "ENetConnector - open() called");

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

  if (mENetHost == nullptr) {
    log_message(ERR, "Could not start ENetHost. Is the port already in use?");
  } else {
    log_message(INFO,"Listening on port: " + std::to_string(mPort));
  }
}

void
ENetConnector::close()
{
  log_message(DEBUG, "ENetConnector - closing on port" + std::to_string(mPort));
  peers.clear();
  if(mENetHost != nullptr)
    enet_host_destroy(mENetHost);
  enet_deinitialize();
}

ENetConnector::~ENetConnector() {
  log_message(DEBUG, "ENetConnector - destructor called");
}

int
ENetConnector::findIdFromPeer(ENetPeer* peer)
{
  for(auto p : peers) {
    if(p.second == peer)
      return p.first;
  }
  return -1;
}

void
ENetConnector::sendPacket(ENetPeer* peer, ENetPacket* packet, enet_uint8 channel)
{
  enet_peer_send(peer, channel, packet);
  enet_host_flush(mENetHost);
}

void
ENetConnector::sendEvent(std::shared_ptr<AbstractEvent> event, int to_id) {
  if(to_id < 0) {
    log_message(INFO, "Attempted to sendEvent to debug player");
    return;
  }
  ENetPeer *to = peers[to_id];
  if(to == nullptr)
    throw std::invalid_argument("Attempted to sendEvent to player ("
                                + std::to_string(to_id)
                                + ") with no ENetPeer");

  std::stringstream blob;
  {
    cereal::PortableBinaryOutputArchive outputArchive(blob);
    outputArchive(event);
  }

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
  enet_host_flush(mENetHost);
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
  peers[newId] = enet_host_connect(mENetHost, &addr, 1, 0);

  ENetEvent event;
  std::stringstream msg;
  msg << "ENet: Connection to " << address.c_str() << " : " << port;
  if (enet_host_service(mENetHost, &event, 5000) > 0 &&
      event.type == ENET_EVENT_TYPE_CONNECT) {
    msg << " succeeded";
    log_message(DEBUG, msg.str());
    return newId;
  } else {
    msg << " failed";
    log_message(DEBUG, msg.str());
    peers.erase(newId);
    return -1;
  }
}

void
ENetConnector::disconnectPeer(int id, std::string reason) {
  if(!peers.count(id))
    return;

  ENetPeer *peer = peers[id];

  /* First send a kick event */
  if(_server) {
    std::unique_ptr<AbstractEvent> k_event(new KickEvent(reason));
    sendEvent(std::move(k_event), id);
  }

  /* Now disconnect the peer */
  enet_peer_disconnect(peer, 0);
  enet_peer_reset(peer);

  return;
}

void
ENetConnector::parseENetPacket(ENetEvent &event,
                               std::shared_ptr<AbstractEvent> &abstractEvent) {
  if (std::strcmp(reinterpret_cast<const char*>(event.packet->data),
                  "ping") == 0) {
    log_message(DEBUG, "Recieved ping command from peer");

    ENetPacket* pong_packet = enet_packet_create(
      "pong", strlen("pong") + 1, ENET_PACKET_FLAG_RELIABLE);
    sendPacket(event.peer, pong_packet, 0);
    log_message(0, "Sent Packet to Peer");
    enet_host_flush(mENetHost);
    return;
  }
  std::stringstream data_in;
  data_in.write((char*)event.packet->data, event.packet->dataLength);

  try {
    std::shared_ptr<AbstractEvent> receive_event;
    cereal::PortableBinaryInputArchive inArchive(data_in);
    inArchive(receive_event);

    abstractEvent = receive_event;
    log_message(DEBUG, "ENetConnector - got event of type " +
                         std::to_string(receive_event->getType()));
  } catch (std::exception& e) {
    std::stringstream strstream;
    strstream << "Received malformed network event.\n" << e.what();
    log_message(ERR, strstream.str());
  }

  /* Clean up the packet now that we're done using it. */
  enet_packet_destroy(event.packet);
}

std::list<std::pair<int, std::shared_ptr<AbstractEvent>>>
ENetConnector::poll(int timeout) {
  if(mENetHost == nullptr) {
    fprintf(stderr, "Need to call configure() and open() on connector before"
                    " opening connections\n");
  }

  // Poll for events and store as { peer_id, shared_ptr<AbstractEvent> }
  std::list<std::pair<int, std::shared_ptr<AbstractEvent>>> events = cache;
  cache.clear();

  ENetEvent event;
  while (enet_host_service(mENetHost, &event, (enet_uint32) timeout) > 0) {
    switch (event.type) {
      case ENET_EVENT_TYPE_RECEIVE: {
        int id = findIdFromPeer(event.peer);
        if(id == -1) {
          log_message(DEBUG, "ENetConnector - Unrecognised peer, registering");
          id = nextFreeId();
          peers[id] = event.peer;
          /*  Set the timeout for the new peer (in ms)*/
          enet_peer_timeout(event.peer, 1000, 0, 0);
        }

        std::shared_ptr<AbstractEvent> abstractEvent;
        parseENetPacket(event, abstractEvent);
        if(abstractEvent != nullptr)
          events.emplace_back(id, abstractEvent);
        break;
      }
      case ENET_EVENT_TYPE_CONNECT: {
        log_message(0, "User made ENet connection");
        std::stringstream message;
        continue;
      }
      case ENET_EVENT_TYPE_DISCONNECT: {
        log_message(INFO, "Disconnect event received");

        /* Reset the peer's client information. */
        event.peer->data = NULL;

        int id = findIdFromPeer(event.peer);
        if(id >= 0) {
          peers.erase(id);
          std::shared_ptr<AbstractEvent> sp_to_handle =
            std::make_shared<PlayerLeaveEvent>();
          events.emplace_back(id, sp_to_handle);
        }
      }
      default:
        break;
    }
  }

  return events;
}


std::pair<int, std::shared_ptr<AbstractEvent>>
ENetConnector::pollFor(int timeout, std::set<EventType> &lookFor) {
  /*
   * Used when the next event is expected to be a particular type, caches
   * everything else.
   */
  std::pair<int, std::shared_ptr<AbstractEvent>> ret;

  ENetEvent event;
  while (enet_host_service(mENetHost, &event, (enet_uint32) timeout) > 0) {
    switch (event.type) {
      case ENET_EVENT_TYPE_RECEIVE: {
        int id = findIdFromPeer(event.peer);
        if(id == -1) {
          log_message(DEBUG, "ENetConnector - Unrecognised peer, registering");
          id = nextFreeId();
          peers[id] = event.peer;
          /*  Set the timeout for the new peer (in ms)*/
          enet_peer_timeout(event.peer, 1000, 0, 0);
        }

        std::shared_ptr<AbstractEvent> abstractEvent;

        parseENetPacket(event, abstractEvent);
        if(abstractEvent != nullptr) {
          if (lookFor.count(abstractEvent->getType()) > 0)
            return std::make_pair(id, abstractEvent);

          printf("Poll for stored event of type %i in cache\n", abstractEvent->getType());
          cache.emplace_back(id, abstractEvent);
        }
      }
      case ENET_EVENT_TYPE_CONNECT: {
        log_message(0, "User made ENet connection");
        std::stringstream message;
      }
      case ENET_EVENT_TYPE_DISCONNECT: {
        log_message(INFO, "Disconnect event received");

        /* Reset the peer's client information. */
        event.peer->data = NULL;

        int id = findIdFromPeer(event.peer);
        if(id >= 0) {
          peers.erase(id);
          std::shared_ptr<AbstractEvent> sp_to_handle =
            std::make_shared<PlayerLeaveEvent>();
          cache.emplace_back(id, sp_to_handle);
        }
      }
      default:
        continue;
    }
  }

  return std::make_pair(-1, nullptr);
}

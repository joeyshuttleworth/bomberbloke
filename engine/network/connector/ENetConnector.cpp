#include "Connector.hpp"

#include <enet/enet.h>
#include <cassert>

#include "KickEvent.hpp"
#include "PlayerLeaveEvent.hpp"
#include "Archive.hpp"

#define ENET_SERVICE_TIMEOUT_MS 50

class ENetConnector : public Connector {
protected:
    ConnectorDescriptor desc;
    bool isOpen = false;

    std::map<ConnectorPeer, ENetPeer*> peers; 

    ENetHost *host = nullptr;
    ENetAddress addr;

    std::list<EventReceived> cache;

    // Services the enet host, needs to be called 'regularly' to keep things moving
    void serviceENet(int);
    
    // Business logic for what to do with ENetEvent
    void processENetEvent(ENetEvent &event, EventReceived &recieved);
public:
    ENetConnector(ConnectorDescriptor desc): desc(desc) {};
    ~ENetConnector();

    bool open() override;
    void close() override;
    void sendEvent(std::shared_ptr<AbstractEvent> event, ConnectorPeer to_id) override;
    void disconnectPeer(ConnectorPeer id, std::string reason) override;
    int latency(ConnectorPeer id) override;
    std::list<EventReceived> poll(int) override;
    EventReceived pollFor(int timeout, std::set<EventType>& lookFor) override;
    int countPeers() override;
    void broadcastEvent(std::shared_ptr<AbstractEvent> event) override;
};

// Implementing the creator method
std::unique_ptr<Connector> createConnector(ConnectorDescriptor desc) {
    return std::unique_ptr<ENetConnector>(new ENetConnector(desc));
}

ENetConnector::~ENetConnector() {
    if(isOpen)
        close();
}

bool
ENetConnector::open() {
    if(enet_initialize() != 0) {
        printf("FATAL: Could not init enet\n");
        return false;
    }

    if(desc.mode == CLIENT) {
        host = enet_host_create(nullptr, 1, 1, 0, 0);
    } else {
        addr.host = ENET_PORT_ANY;
        addr.port = desc.listenPort;
        host = enet_host_create(&addr, 124, 10, 0, 0);
    }

    if(host == nullptr) {
        printf("Could not start ENetHost. Is the port already in use?\n");
        return false;
    }

    isOpen = true;
    if(desc.mode == CLIENT) {
        if(enet_address_set_host(&addr, desc.serverAddress.c_str()) < 0) {
            printf("Failed to resolve hostname\n");
            close();
            return false;
        }
        addr.port = desc.serverPort;

        int newId = rand();
        peers[newId] = enet_host_connect(host, &addr, 1, 0);

        ENetEvent event;
        std::stringstream msg;
        msg << "ENet: Connection to " << desc.serverAddress.c_str() << " : " << desc.serverPort;
        if (enet_host_service(host, &event, 5000) > 0 &&
            event.type == ENET_EVENT_TYPE_CONNECT) {
            msg << " succeeded";
            printf("%s\n", msg.str().c_str());
        } else {
            msg << " failed";
            printf("%s\n", msg.str().c_str());
            close();
            return false;
        }

        // poll now allows us to service any handshakes
        cache = poll(0);
    }

    return true;
}

void
ENetConnector::close() {
    if(!isOpen)
        throw std::runtime_error("Connector: tried to close() but was not open");

    // Clear out stored EventRecieved's
    cache.clear();
    peers.clear();

    // ENet clearup
    if(host != nullptr)
        enet_host_destroy(host);
    enet_deinitialize();

    isOpen = false;
}

void ENetConnector::serviceENet(int timeout) {
    //printf("enet_host_service timeout=%i\n", timeout);
    ENetEvent event;
    int result;
    while((result = enet_host_service(host, &event, (enet_uint32) timeout)) != 0) {
        if(result < 0) // Err
            throw std::runtime_error("enet_host_service failure");

        EventReceived recieved;
        processENetEvent(event, recieved);
        if(recieved != EVENT_RECEIVED_NONE)
            cache.push_back(recieved); 
    }
}

void ENetConnector::processENetEvent(ENetEvent &event, EventReceived &recieved)
{
    recieved = EVENT_RECEIVED_NONE;

    // Recover ConnectorPeer ID
    ConnectorPeer from = event.peer->data == NULL ? -1 : *((ConnectorPeer*) event.peer->data);
    
    // Business logic
    switch(event.type) {
        case ENET_EVENT_TYPE_CONNECT: {
            printf("%i ENet : Connect %i\n", desc.mode, (int) event.peer->state);
            assert(from == -1); // Should be new

            // Generate new ID
            from = rand();
            while(peers.count(from))
                from = rand();

            // Store
            peers[from] = event.peer;
            event.peer->data = new ConnectorPeer(from);

            /* Could configure ping-pong and timeouts here but the defaults are fine */
            //enet_peer_ping_interval(event.peer, 1);
            //enet_peer_timeout(event.peer, 2, 0, 500);
            break;
        }
        case ENET_EVENT_TYPE_DISCONNECT: {
            printf("%i Enet : Disconnect\n", desc.mode);

            /* Reset the peer's client information. */
            delete (ConnectorPeer *) event.peer->data;
            event.peer->data = NULL;

            if(from >= 0) {
                peers.erase(from);
                recieved = { std::make_shared<PlayerLeaveEvent>(), from };
            }
            break;
        }
        case ENET_EVENT_TYPE_RECEIVE: {
            //printf("%i Enet : Receieve\n", desc.mode);

            // Parse AbstractEvent
            std::stringstream data_in;
            data_in.write((char*)event.packet->data, event.packet->dataLength);
            try {
                std::shared_ptr<AbstractEvent> receive_event;
                CEREAL_INPUT_ARCHIVE inArchive(data_in);
                inArchive(receive_event);
                recieved = { receive_event, from };
            } catch (std::exception& e) {
                std::stringstream strstream;
                strstream << "Received malformed network event.\n" << e.what();
                printf("%s\n", strstream.str().c_str());
            }
            enet_packet_destroy(event.packet);
            break;
        }
        default:
            break;
    }
}

void 
ENetConnector::sendEvent(std::shared_ptr<AbstractEvent> event, ConnectorPeer to_id) {
    if(to_id < 0) {
        printf("Attempted to sendEvent to debug player\n");
        return;
    }
    ENetPeer *to = peers[to_id];
    if(to == nullptr)
        throw std::invalid_argument("Attempted to sendEvent to player ("
                                    + std::to_string(to_id)
                                    + ") with no ENetPeer");

    std::stringstream blob;
    {
        CEREAL_OUTPUT_ARCHIVE outputArchive(blob);
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
    enet_host_flush(host);
};

void 
ENetConnector::broadcastEvent(std::shared_ptr<AbstractEvent> event) {
    for(auto peer : peers)
        sendEvent(event, peer.first);
};

void 
ENetConnector::disconnectPeer(ConnectorPeer id, std::string reason) {
    if(!peers.count(id))
        return;

    ENetPeer *peer = peers[id];

    /* First send a kick event */
    if(desc.mode == SERVER) {
        auto k_event = std::make_shared<KickEvent>(reason);
        sendEvent(k_event, id);
    }

    /* Now disconnect the peer */
    enet_peer_disconnect(peer, 0);
    enet_peer_reset(peer);
    peers.erase(id);

    /* If on the server emit a PlayerLeaveEvent */
    if(desc.mode == SERVER)
        cache.push_back({std::make_shared<PlayerLeaveEvent>(), id});
};

int 
ENetConnector::latency(ConnectorPeer id) {
    if(peers.count(id) == 0)
        return -1;
    return (int) peers[id]->lastRoundTripTime;
};

std::list<EventReceived> 
ENetConnector::poll(int timeout) {
    std::list<EventReceived> events = cache;
    cache.clear();

    auto now = std::chrono::steady_clock::now();
    const auto endTime = now + std::chrono::milliseconds(timeout);
    do {
        int left = timeout == 0 ? 0 : std::chrono::duration_cast<std::chrono::milliseconds>(endTime - now).count();
        serviceENet(std::min(left, ENET_SERVICE_TIMEOUT_MS));

        if(!cache.empty()) {
            events.insert(events.end(), cache.begin(), cache.end());
            cache.clear();
        }
        if(!events.empty())
            break;

    } while((now = std::chrono::steady_clock::now()) < endTime);

    return events;
};

EventReceived 
ENetConnector::pollFor(int timeout, std::set<EventType>& lookFor) {
    // Finds and removes matching event
    auto scanCache = [&]() {
        for(auto it = cache.begin(); it != cache.end(); it++) {
            if(lookFor.count((*it).event->getType()) > 0) {
                EventReceived copy = *it;
                cache.erase(it);
                return copy;
            }
        }
        return EVENT_RECEIVED_NONE;
    };

    auto now = std::chrono::steady_clock::now();
    const auto endTime = now + std::chrono::milliseconds(timeout);
    do {
        int left = timeout == 0 ? 0 : std::chrono::duration_cast<std::chrono::milliseconds>(endTime - now).count();
        serviceENet(std::min(left, ENET_SERVICE_TIMEOUT_MS));

        auto got = scanCache();
        if(got != EVENT_RECEIVED_NONE)
            return got;
    } while((now = std::chrono::steady_clock::now()) < endTime);

    return EVENT_RECEIVED_NONE;
};

int 
ENetConnector::countPeers() {
    return (int) peers.size();
};

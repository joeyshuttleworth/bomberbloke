#include "Connector.hpp"

#include <emscripten/websocket.h>
#include <emscripten/emscripten.h>
#include <cassert>

#include "Archive.hpp"


class EmscriptenConnector : public Connector {
protected:
    ConnectorDescriptor desc;
    bool isOpen = false;

    EMSCRIPTEN_WEBSOCKET_T socket;

    std::list<EventReceived> cache;
    std::string serverUrl();
public:
    // Callbacks forward events to the cache through here
    void event(EventReceived &recieved) { cache.push_back(recieved); };

    EmscriptenConnector(ConnectorDescriptor desc): desc(desc) {};
    ~EmscriptenConnector();
    
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

std::unique_ptr<Connector> 
createConnector(ConnectorDescriptor desc) {
    // Web browsers cannot act as websocket servers
    assert(desc.mode != SERVER);

    return std::unique_ptr<Connector>(new EmscriptenConnector(desc));
}

std::string
EmscriptenConnector::serverUrl() {
    std::string url = "";
    const auto WS_PREFIX = "ws://";

    // Correct prefix
    if(desc.serverAddress.rfind(WS_PREFIX, 0) == desc.serverAddress.npos)
        url = url + WS_PREFIX;

    // hostname:port
    url = url + desc.serverAddress + ":" + std::to_string(desc.serverPort);

    return url;
}

EmscriptenConnector::~EmscriptenConnector() {
    if(isOpen)
        close();
}

bool
EmscriptenConnector::open() {
    assert(emscripten_websocket_is_supported());

    /*
    * Create websocket and setup callbacks
    */

    EmscriptenWebSocketCreateAttributes attr;
    attr.url = serverUrl().c_str();
    attr.protocols = "bloke-ws-protocol";
    attr.createOnMainThread = true;

    socket = emscripten_websocket_new(&attr);
    if(socket <= 0) {
        printf("Websocket connection creation has failed (%i)\n", socket);
        return false;
    }

    emscripten_websocket_set_onopen_callback(socket, this, [](
        int eventType, const EmscriptenWebSocketOpenEvent *event, void *userData
    ){
        printf("Emscripten onOpen callback\n");
        return EM_TRUE;
    });
    emscripten_websocket_set_onerror_callback(socket, this, [](
        int eventType, const EmscriptenWebSocketErrorEvent *event, void *userData
    ){
        printf("Emscripten onError callback\n");
        return EM_TRUE;
    });
    emscripten_websocket_set_onclose_callback(socket, this, [](
        int eventType, const EmscriptenWebSocketCloseEvent *event, void *userData
    ){
        printf("Emscripten onClose callback\n");
        auto conn = (EmscriptenConnector*) userData;
        conn->close();
        return EM_TRUE;
    });
    emscripten_websocket_set_onmessage_callback(socket, this, [](
        int eventType, const EmscriptenWebSocketMessageEvent *event, void *userData
    ){
        #ifdef BLOKE_PROTOCOL_USE_JSON
            // In this mode we expect to be receiving UTF-8 encoded messages
            if(!event->isText) {
                printf("Emscripten onMessage got non UTF-8 message\n");
                return EM_FALSE;
            }
        #endif

        // Get
        auto conn = (EmscriptenConnector*) userData;
        std::stringstream s;
        s.write((char *) event->data, event->numBytes);

        // Parse
        std::shared_ptr<AbstractEvent> receive_event;
        CEREAL_INPUT_ARCHIVE inArchive(s);
        inArchive(receive_event);

        // Store
        EventReceived rec;
        rec.event = receive_event;
        rec.from = 1;
        conn->event(rec);

        return EM_TRUE;
    });

    /*
    * async wait to verify connection
    */

    int waits = 0;
    while(countPeers() != 1) {
        if(waits == 10) {
            printf("Timed out\n");
            return false;
        }
        emscripten_sleep(100);
        waits++;
    }
    printf("EmscriptenConnector: Connection appears to be open!\n");

    isOpen = true;
    return true;
}

void
EmscriptenConnector::close() {
    printf("EmscriptenConnector::close()\n");
    cache.clear();
    emscripten_websocket_deinitialize();
    isOpen = false;
}

void 
EmscriptenConnector::sendEvent(std::shared_ptr<AbstractEvent> event, ConnectorPeer to_id) {
    (void) to_id;
    broadcastEvent(event);
};

void  
EmscriptenConnector::disconnectPeer(ConnectorPeer id, std::string reason) {
    (void) id;
    (void) reason;
    return;
};

int 
EmscriptenConnector::latency(ConnectorPeer id) {
    return 0; // TODO
};

std::list<EventReceived> 
EmscriptenConnector::poll(int timeout) {
    if(timeout > 0)
        printf("EmscriptenConnector::poll(timeout) for timeout > 0 is not implemented\n");

    auto recieved = cache;
    cache.clear();
    return recieved;
};

EventReceived 
EmscriptenConnector::pollFor(int timeout, std::set<EventType>& lookFor) {
    if(!isOpen)
        return EVENT_RECEIVED_NONE;

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
        auto got = scanCache();
        if(got != EVENT_RECEIVED_NONE)
            return got;
        emscripten_sleep(50);
    } while((now = std::chrono::steady_clock::now()) < endTime);

    return EVENT_RECEIVED_NONE;
};

int 
EmscriptenConnector::countPeers() {
    if(socket <= 0)
        return 0;

    //https://developer.mozilla.org/en-US/docs/Web/API/WebSocket/readyState
    unsigned short state;
    emscripten_websocket_get_ready_state(this->socket, &state);

    if(state == 1) // OPEN
        return 1;
    return 0;
};

void 
EmscriptenConnector::broadcastEvent(std::shared_ptr<AbstractEvent> event) {
    if(!isOpen)
        return;

    // Connector is client only so just send to server
    std::stringstream blob;
    {
        CEREAL_OUTPUT_ARCHIVE outputArchive(blob);
        outputArchive(event);
    }
    std::string bytes = blob.str();

    #ifdef BLOKE_PROTOCOL_USE_JSON
        emscripten_websocket_send_utf8_text(socket, bytes.c_str());
    #else
        emscripten_websocket_send_binary(socket, (void *) bytes.c_str(), bytes.size());
    #endif
};
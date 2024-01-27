#include "Connector.hpp"

#include <libwebsockets.h>

#include <atomic>
#include <memory>
#include <thread>
#include <sstream>
#include <list>
#include <map>
#include <vector>
#include <cassert>

#include "PlayerLeaveEvent.hpp"
#include "KickEvent.hpp"
#include "Archive.hpp"

using std::list;
using std::map;
using std::vector;

/*
* General notes on LWS development:
* Contrary to the official docs, the ONLY threadsafe method is lws_cancel_service(...)
* which invokes the callback with LWS_CALLBACK_EVENT_WAIT_CANCELLED on the lws_service
* thread. This is the only proper way to communicate with it in a multithreaded context.
*
* To send a message over web sockets, you must invoke lws_callback_on_writeable(wsi*) and
* then wait for the oppertunity when passed a LWS_CALLBACK_SERVER_WRITEABLE (or CLIENT) to
* finally call lws_write... 
*/

//#define DEBUG_LWS 0
#define DEBUG_LWS 0

/*
* libwebsockets requires some setup 
*/
namespace lws_config {
    /*
    * Browser currently only supports JSON UTF-8 which is more ineffecient than binary, so need larger packets
    */
    #ifdef BLOKE_PROTOCOL_USE_JSON
        #define BLOKE_RX_BUFFER_BYTES (65536)
    #else
        #define BLOKE_RX_BUFFER_BYTES (4096)
    #endif

    /*
    * The sub protocol name, this is part of the Web Sockets Protocol
    */
    #define BLOKE_PROTOCOL_NAME "bloke-ws-protocol"

    /*
    * Sleep time when waiting for the libwebsockets thread to produce messages
    */
    #define POLL_WAIT_MILLISECONDS 50

    const static char protocol_name[] = BLOKE_PROTOCOL_NAME;

    struct payload {
        unsigned char data[LWS_SEND_BUFFER_PRE_PADDING + BLOKE_RX_BUFFER_BYTES];
        size_t len;
    };

    typedef struct per_session_data {
        int id;
    } per_session_data;

    static const uint32_t backoff_ms[] = { 1000, 2000, 3000, 4000, 5000 };
    static const lws_retry_bo_t retry = {
        backoff_ms,
        LWS_ARRAY_SIZE(backoff_ms), //
        LWS_ARRAY_SIZE(backoff_ms), //
        60,  /* force PINGs after secs idle */
        70, /* hangup after secs idle */
        20,
    };

}

/*
* Our implementation of the Connector interface
*/
class WebSocketsConnector : public Connector {
protected:
    ConnectorDescriptor desc;
    bool isOpen = false;
    lws_context *context;
    vector<lws_protocols> protocols;

    std::mutex peersMutex;
    map<ConnectorPeer, lws*> peers;

    // lws_service lives on this thread when running;
    std::mutex cancel_service_mutex;
    std::atomic<bool> shouldRun;
    std::thread lwsThread;

    // Communicating with lws thread to receive events
    std::mutex incomingMutex;
    list<EventReceived> incoming;

    // Communicating with lws thread to send events
    std::mutex outgoingMutex;
    map<int, list<std::shared_ptr<AbstractEvent>>> outgoing;

    void startLwsThread();
    bool waitForPeers(int nPeers, int timeoutSeconds);
public:
    WebSocketsConnector(ConnectorDescriptor desc): desc(desc) {};
    ~WebSocketsConnector();

    int lws_callback_member(struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len);

    bool open() override;
    void close() override;
    void sendEvent(std::shared_ptr<AbstractEvent> event, ConnectorPeer to_id) override;
    void disconnectPeer(ConnectorPeer id, std::string reason) override;
    int latency(ConnectorPeer id) override;
    list<EventReceived> poll(int) override;
    EventReceived pollFor(int timeout, std::set<EventType>& lookFor) override;
    int countPeers() override;
    void broadcastEvent(std::shared_ptr<AbstractEvent> event) override;
};

// Implementing the creator method
std::unique_ptr<Connector> createConnector(ConnectorDescriptor desc) {
    return std::unique_ptr<WebSocketsConnector>(new WebSocketsConnector(desc));
}

void WebSocketsConnector::startLwsThread() {
    // Run in thread, will keep going until shouldRun = false
    shouldRun.store(true);
    lwsThread = std::thread([&](){
        while(shouldRun.load()) 
            lws_service(context, 0);
    });
}

bool WebSocketsConnector::waitForPeers(int nPeers, int timeoutSeconds) {
    // Block until at least nPeers or timeout
    const auto timeout = std::chrono::steady_clock::now() + std::chrono::seconds(timeoutSeconds);
    while(this->countPeers() < nPeers) {
        if(std::chrono::steady_clock::now() < timeout)
            std::this_thread::sleep_for(std::chrono::milliseconds(POLL_WAIT_MILLISECONDS));
        else {
            return false;
        }
    }
    return true;
}

WebSocketsConnector::~WebSocketsConnector() {
    if(isOpen)
        close();
}

int WebSocketsConnector::lws_callback_member(struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len) {
    auto data = (lws_config::per_session_data *) user;
    (void) in;
    (void) len;

    switch( reason )
    {
        case LWS_CALLBACK_CLIENT_ESTABLISHED:
        case LWS_CALLBACK_ESTABLISHED: {
            if(DEBUG_LWS)
                printf("LWS: ESTABLISHED\n");

            // Assign id
            const std::lock_guard<std::mutex> lock(peersMutex);

            ConnectorPeer id = abs(rand());
            while(peers.count(id) > 0)
                id = abs(rand());

            const std::lock_guard<std::mutex> lock_2(outgoingMutex);

            peers[id] = wsi;
            *data = lws_config::per_session_data{ id };

            outgoing[id] = list<std::shared_ptr<AbstractEvent>>();
            break;
        }
        case LWS_CALLBACK_CLIENT_CLOSED: 
        case LWS_CALLBACK_CLOSED: {
            if(DEBUG_LWS)
                printf("LWS: CLOSED\n");

            {
                const std::lock_guard<std::mutex> lock(peersMutex);
                const std::lock_guard<std::mutex> lock_2(outgoingMutex);
                // Free id
                peers.erase(data->id);
                // Delete outgoing queue
                outgoing.erase(data->id);
            }
            if(desc.mode == SERVER) {
                // Emit PlayerLeaveEvent
                const std::lock_guard<std::mutex> lock(incomingMutex);
                incoming.push_back({std::make_shared<PlayerLeaveEvent>(), data->id});
            }
            return 0;
        }
        case LWS_CALLBACK_CLIENT_RECEIVE:
        case LWS_CALLBACK_RECEIVE: {
            if(DEBUG_LWS)
                printf("LWS: RECEIVE\n");

            auto inputString = std::string((char *) in, len);

            // Parse
            std::stringstream s;
            s.write((char *) in, len);
            try {
                std::shared_ptr<AbstractEvent> receive_event;
                CEREAL_INPUT_ARCHIVE inArchive(s);
                inArchive(receive_event);

                const std::lock_guard<std::mutex> lock(incomingMutex);
                incoming.push_back({receive_event, data->id});
            } catch (std::exception& e) {
                std::stringstream strstream;
                strstream << "Received malformed network event.\n" << e.what();
                printf("Error deserialising\n");
                return -1; // kill connection
            }
            break;
        }
        case LWS_CALLBACK_CLIENT_WRITEABLE: 
        case LWS_CALLBACK_SERVER_WRITEABLE: {
            if(DEBUG_LWS)
                printf("LWS: WRITEABLE\n");
                
            std::shared_ptr<AbstractEvent> event;
            {
                const std::lock_guard<std::mutex> lock(outgoingMutex);
                size_t n_queued = outgoing.at(data->id).size();
                if(n_queued == 0)
                    return 0;
                else if(n_queued > 1)
                    lws_callback_on_writable(wsi); // More queued

                event = outgoing[data->id].front(); // copy
                outgoing[data->id].pop_front();
            }

            std::stringstream blob;
            {
                CEREAL_OUTPUT_ARCHIVE outputArchive(blob);
                outputArchive(event);
            }

            assert((int) blob.str().size() < BLOKE_RX_BUFFER_BYTES);

            std::string str(LWS_SEND_BUFFER_PRE_PADDING, ' '); // LWS_PRE bytes
            str += blob.str();
            auto result = lws_write(wsi,
                            (unsigned char *) &str[LWS_SEND_BUFFER_PRE_PADDING],
                            str.size() - LWS_SEND_BUFFER_PRE_PADDING, 
                            LWS_WRITE_TEXT);
            
            if(EVENT_KICK == event->getType()) 
                return -1; // Disconnect peer 

            bool successfulSend = result >= 0;
            return successfulSend ? 0 : -1; // Kill connection if something went wrong 
        }
        case LWS_CALLBACK_GET_THREAD_ID: {
            uint64_t tid;
            #ifdef __APPLE__        
                pthread_threadid_np(NULL, &tid);
            #else
                tid = pthread_self();
            #endif
            return tid;
        }
        case LWS_CALLBACK_EVENT_WAIT_CANCELLED: {
            // Interrupt, if messages to send - schedule the necessary callback 
            const std::lock_guard<std::mutex> lock(cancel_service_mutex);
            const std::lock_guard<std::mutex> lock_2(outgoingMutex);
            const std::lock_guard<std::mutex> lock_3(peersMutex);

            for(auto &out : outgoing)
                if(out.second.size() != 0)
                    lws_callback_on_writable(peers.at(out.first));

            return 0;
        }
        default:
            break;
    }
    return 0;
}

bool WebSocketsConnector::open() {
    if(DEBUG_LWS) // Note: flags are pointless if LWS has not been built from sources with the logging levels selected
        lws_set_log_level(LLL_ERR | LLL_WARN | LLL_NOTICE | LLL_INFO | LLL_DEBUG | LLL_USER | LLL_THREAD, NULL);
    else
        lws_set_log_level((unsigned int) 0, nullptr);

    struct lws_context_creation_info info;
    memset( &info, 0, sizeof(info) );

    if(desc.mode == CLIENT)
        info.port = CONTEXT_PORT_NO_LISTEN;
    else
        info.port = desc.listenPort;
    
    // We define a single protocol to handle messages
    protocols = 
    {
        {
            lws_config::protocol_name,
            [](struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len){
                auto connector = (WebSocketsConnector *) lws_context_user(lws_get_context(wsi));
                if(connector) 
                    return connector->lws_callback_member(wsi, reason, user, in, len);

                throw std::runtime_error("Libwebsockets callback could not find connector to forward to\n");
            },
            sizeof(lws_config::per_session_data),
            BLOKE_RX_BUFFER_BYTES,
            0,
            nullptr, 
            BLOKE_RX_BUFFER_BYTES
        },
        LWS_PROTOCOL_LIST_TERM 
    };

    info.protocols = protocols.data();
    info.gid = -1;
    info.uid = -1;
    info.user = this;
    if(desc.mode == SERVER)
        info.options = LWS_SERVER_OPTION_HTTP_HEADERS_SECURITY_BEST_PRACTICES_ENFORCE;
    else
        info.options = 0;

    context = lws_create_context( &info );

    if(desc.mode == CLIENT) {
        struct lws_client_connect_info i;
        memset(&i, 0, sizeof(i));

        std::string protocol_name = BLOKE_PROTOCOL_NAME;

        i.context = context;
        i.port = desc.serverPort;
        i.address = desc.serverAddress.c_str();
        i.path = "/";
        i.host = i.address;
        i.origin = i.address;
        i.ssl_connection = 0;
        i.protocol = protocol_name.c_str();
        i.local_protocol_name = protocol_name.c_str();
        i.pwsi = nullptr;
        i.retry_and_idle_policy = &lws_config::retry;
        i.userdata = nullptr;

        if (!lws_client_connect_via_info(&i)) {
            printf("Connector: lws_client_connect_via_info failed\n");
            lws_context_destroy(context);
            return false;
        }
    }

    isOpen = true;
    this->startLwsThread();
    
    if(desc.mode == SERVER)
        return true;
    else {
        // CLIENT mode means we must have the server as a peer by the time this function returns
        bool connected = this->waitForPeers(1, 15);
        if(connected)
            return true;
        else {
            this->close();
            return false;
        }
    }
}

void WebSocketsConnector::close() {
    if(!isOpen)
        throw std::runtime_error("Connector: tried to close() but was not open");
    isOpen = false;

    // Manage ending of lws main thread
    shouldRun.store(false);
    lws_cancel_service(context); // make lws_service return
    lwsThread.join(); // Wait for thread to die

    lws_context_destroy(context); // destroy
    protocols.clear();
}

void
WebSocketsConnector::disconnectPeer(ConnectorPeer id, std::string reason)
{
    // Our callback will disconnect the peer when it sees a KickEvent
    auto event = std::make_shared<KickEvent>(reason);
    sendEvent(event, id);
}

int
WebSocketsConnector::latency(ConnectorPeer id)
{
    (void) id;
    // TODO
    return 0;
}

list<EventReceived>
WebSocketsConnector::poll(int timeout) {
    if(timeout == 0) {
        const std::lock_guard<std::mutex> lock(incomingMutex);
        auto received = incoming; // copy
        incoming.clear();
        return received;
    }

    const auto start = std::chrono::steady_clock::now();
    const auto finish = start + std::chrono::milliseconds(timeout);
    while(std::chrono::steady_clock::now() < finish) {
        {
            const std::lock_guard<std::mutex> lock(incomingMutex);
            if (!incoming.empty()) {
                auto received = incoming; // copy
                incoming.clear();
                return received;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(POLL_WAIT_MILLISECONDS));
    }
    return {};
}

EventReceived
WebSocketsConnector::pollFor(int timeout, std::set<EventType>& lookFor) {
    EventReceived found = EVENT_RECEIVED_NONE;
    {
        const std::lock_guard<std::mutex> lock(incomingMutex);
        for (auto it = incoming.begin(); it != incoming.end(); it++) {
            if (lookFor.count((*it).event->getType()) != 0) {
                found = *it;
                incoming.erase(it); // Pop
                return found;
            }
        }
    }

    if(timeout <= 0)
        return found;

    std::this_thread::sleep_for(std::chrono::milliseconds(POLL_WAIT_MILLISECONDS));
    return pollFor(timeout - POLL_WAIT_MILLISECONDS, lookFor);
}
int WebSocketsConnector::countPeers() {
    const std::lock_guard<std::mutex> lock(peersMutex);
    return peers.size();
}

void
WebSocketsConnector::sendEvent(std::shared_ptr<AbstractEvent> event,
                               ConnectorPeer to_id) {
    {
        const std::lock_guard<std::mutex> lock(outgoingMutex);
        if(outgoing.count(to_id) == 0) {
            printf("Err sendEvent, peer not found\n");
            return;
        }
        outgoing.at(to_id).push_back(event);
    }
    
    // Interrupt LWS thread
    const std::lock_guard<std::mutex> lock(cancel_service_mutex);
    lws_cancel_service(context);
}

void
WebSocketsConnector::broadcastEvent(std::shared_ptr<AbstractEvent> event)
{
    {
        const std::lock_guard<std::mutex> lock(outgoingMutex);
        for(auto &key : outgoing)
            outgoing[key.first].push_back(event);
    }

    // Interrupt LWS thread
    const std::lock_guard<std::mutex> lock(cancel_service_mutex);
    lws_cancel_service(context);
}

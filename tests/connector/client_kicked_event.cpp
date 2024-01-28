#include <thread>
#include <memory>
#include <cassert>
#include <Connector.hpp>
#include <CommandEvent.hpp>

/*
* client_kicked_event
* Tests both server-side/client-side connector behaviour on being kicked. The Client
* should recieve a PlayerKickEvent.
*/

const std::string KICK_REASON = "Testing kick functionality";


void run_server() {
  ConnectorDescriptor desc;
  desc.mode = SERVER;
  desc.listenPort = 8888;

  auto conn = createConnector(desc);

  assert(conn->open()); // Server started

  std::set<EventType> lookFor = { EVENT_COMMAND };
  auto got = conn->pollFor(2000, lookFor);

  assert(got.event != nullptr && got.from != -1); // Check received
  assert(EVENT_COMMAND == got.event->getType()); // Check was expected

  // Perform the kick now we have the client ConnectorPeer identifier
  conn->disconnectPeer(got.from, KICK_REASON);

  // We should see a PlayerLeaveEvent server side
  lookFor = { EVENT_PLAYERLEAVE };
  got = conn->pollFor(2000, lookFor);
  assert(got != EVENT_RECEIVED_NONE);
  assert(EVENT_PLAYERLEAVE == got.event->getType());

  std::this_thread::sleep_for(std::chrono::seconds(1));
  conn->close();
}

void run_client() {
  ConnectorDescriptor desc;
  desc.mode = CLIENT;
  desc.serverAddress = "localhost";
  desc.serverPort = 8888;

  auto conn = createConnector(desc);

  assert(conn->open()); // Connection succeeded
 
  // Send initial greeting
  std::shared_ptr<AbstractEvent> initialEvent = std::make_shared<CommandEvent>("hello w0rld?");
  conn->broadcastEvent(initialEvent);

  // Wait for kick event
  auto got = conn->poll(2000);
  assert(got.size() == 1);
  assert(EVENT_KICK == got.front().event->getType());

  conn->close();
}


int
main(int, char**)
{
  std::thread server(run_server);
  std::thread client(run_client);
  server.join();
  client.join();

  printf("End\n");

  return 0;
}

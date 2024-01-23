#include <thread>
#include <memory>
#include <cassert>
#include <Connector.hpp>
#include <MessageEvent.hpp>


/*
* leave_event_server
* Tests server-side connector behaviour, a PlayerLeaveEvent should be emitted on connection close/failure.
*/


void run_server() {
  ConnectorDescriptor desc;
  desc.mode = SERVER;
  desc.listenPort = 8888;

  auto conn = createConnector(desc);

  assert(conn->open()); // Server started

  auto got = conn->poll(30000); // Poll until timeout detected

  // Check server connector emitted PlayerLeaveEvent
  assert(got.size() > 0);
  assert(got.size() == 1);
  assert(EVENT_PLAYERLEAVE == got.front().event->getType());

  conn->close();
}

void run_client() {
  ConnectorDescriptor desc;
  desc.mode = CLIENT;
  desc.serverAddress = "localhost";
  desc.serverPort = 8888;

  auto conn = createConnector(desc);
  assert(conn->open()); // Connection succeeded

  conn->close(); // Should cause server connector to notice and emit PlayerLeaveEvent
  printf("Client closed\n");
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

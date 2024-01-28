#include <thread>
#include <memory>
#include <cassert>
#include <Connector.hpp>
#include <MessageEvent.hpp>


void run_server() {
  ConnectorDescriptor desc;
  desc.mode = SERVER;
  desc.listenPort = 8888;

  auto conn = createConnector(desc);

  assert(conn->open()); // Server started

  // Poll for 2 seconds
  conn->poll(2000);

  // Check there is 1 active connection
  assert(conn->countPeers() == 1);

  printf("Server closing\n");
  conn->close();
}

void run_client() {
  ConnectorDescriptor desc;
  desc.mode = CLIENT;
  desc.serverAddress = "localhost";
  desc.serverPort = 8888;

  auto conn = createConnector(desc);

  assert(conn->open()); // Connection succeeded

  // TODO test with lws, busy poll for 5 seconds
  conn->poll(5000);
  //std::set<EventType> empty = {};
  //conn->pollFor(5000, empty);
  //std::this_thread::sleep_for(std::chrono::seconds(5));

  printf("Client closing\n");
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

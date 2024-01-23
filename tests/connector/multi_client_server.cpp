#include <thread>
#include <memory>
#include <cassert>
#include <Connector.hpp>
#include <MessageEvent.hpp>

const int N_CLIENTS = 5;
const int CLIENT_LIFETIME_SECONDS = 5;


void run_server() {
  ConnectorDescriptor desc;
  desc.mode = SERVER;
  desc.listenPort = 8888;

  auto conn = createConnector(desc);

  assert(conn->open()); // Server started

  // TODO test with lws, poll for 2 seconds
  std::set<EventType> empty = {};
  conn->pollFor(2000, empty);
  ///conn->poll(2000);

  // Check there is the expected number of connections
  assert(conn->countPeers() == N_CLIENTS);

  conn->close();
}

void run_client() {
  ConnectorDescriptor desc;
  desc.mode = CLIENT;
  desc.serverAddress = "localhost";
  desc.serverPort = 8888;

  auto conn = createConnector(desc);

  assert(conn->open()); // Connection succeeded

  // TODO test with lws, enter poll wait
  std::set<EventType> empty = {};
  conn->pollFor(1000 * CLIENT_LIFETIME_SECONDS, empty);
  //std::this_thread::sleep_for(std::chrono::seconds(CLIENT_LIFETIME_SECONDS));

  conn->close();
}


int
main(int, char**)
{
  std::thread server(run_server);

  std::vector<std::thread> threads;
  for(int i = 0; i < N_CLIENTS; i++)
    threads.emplace_back(run_client); // Launch clients
  
  server.join();
  for(auto &thread : threads)
    thread.join();

  printf("End\n");

  return 0;
}

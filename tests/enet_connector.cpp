#include <thread>
#include <ENetConnector.hpp>
#include <MessageEvent.hpp>


ENetConnector server_connector;
ENetConnector client_connector;

void run_server() {
  auto got = server_connector.poll(1000);

  // Check server got message, and that it was serialised correctly
  assert(got.size() == 1);
  assert(EVENT_MESSAGE == got.front().event->getType());
}

void run_client() {
  int serverId = client_connector.connectPeer("localhost", 8888);
  assert(serverId > -1); // Connection succeeded

  std::shared_ptr<AbstractEvent> testEvent = std::make_shared<MessageEvent>("test");
  client_connector.sendEvent(testEvent, serverId);
}

int
main(int, char**)
{
  server_connector.configureListenPort(8888);
  server_connector.open();
  client_connector.open();

  std::thread client(run_client);
  std::thread server(run_server);

  client.join();
  server.join();

  return 0;
}

void new_game(std::string)
{
  return;
}


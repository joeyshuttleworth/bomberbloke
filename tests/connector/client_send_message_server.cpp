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

  auto got = conn->poll(2000);

  // Check server got message, and that it was serialised correctly
  assert(got.size() == 1);
  assert(EVENT_MESSAGE == got.front().event->getType());

  std::shared_ptr<MessageEvent> event = std::dynamic_pointer_cast<MessageEvent>(got.front().event);
  printf("Recovered: %s\n", event->getMessage().c_str());

  conn->close();
}

void run_client() {
  ConnectorDescriptor desc;
  desc.mode = CLIENT;
  desc.serverAddress = "localhost";
  desc.serverPort = 8888;

  auto conn = createConnector(desc);

  assert(conn->open()); // Connection succeeded

  std::shared_ptr<AbstractEvent> testEvent = std::make_shared<MessageEvent>("hello w0rld?");
  conn->broadcastEvent(testEvent);

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

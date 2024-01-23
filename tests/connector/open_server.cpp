#include <thread>
#include <memory>
#include <cassert>
#include <Connector.hpp>


int
main(int, char**)
{
  ConnectorDescriptor desc;
  desc.mode = SERVER;
  desc.listenPort = 8888;

  auto conn = createConnector(desc);
  assert(conn->open()); // Server started
  conn->close();

  return 0;
}

#include "cereal/archives/json.hpp"
#include "cereal/archives/portable_binary.hpp"
#include "QueryEvent.hpp"

int
main(int, char**)
{
  std::shared_ptr<AbstractEvent> event(new QueryEvent("test"));
  printf("ENetConnector::sendEvent (event type is %i) contents in JSON is: \n", event->getType());
  {
    cereal::JSONOutputArchive oArchive(std::cout);
    oArchive(event);
  }
  printf("\nBinary:\n");
  {
    cereal::PortableBinaryOutputArchive outputArchive(std::cout);
    outputArchive(event);
  }

  std::stringstream ss( std::ios::in | std::ios::out | std::ios::binary );
  {
    cereal::PortableBinaryOutputArchive outputArchive(ss);
    outputArchive(event);
  }

  // Attempt to recover
  try {
    std::shared_ptr<AbstractEvent> receive_event;
    cereal::PortableBinaryInputArchive inArchive(ss);
    inArchive(receive_event);

    std::shared_ptr<AbstractEvent> sp_to_handle(std::move(receive_event));
  } catch (std::exception& e) {
    std::stringstream strstream;
    strstream << "Received malformed network event.\n" << e.what();
    std::cout << strstream.str() << std::endl;
  }

  return 0;
}

#include "cereal/archives/portable_binary.hpp"
#include "QueryEvent.hpp"

int
main(int, char**)
{
  std::shared_ptr<AbstractEvent> event(new QueryEvent("test"));
  std::stringstream ss( std::ios::in | std::ios::out | std::ios::binary );
  {
    cereal::PortableBinaryOutputArchive outputArchive(ss);
    outputArchive(event);
  }

  // Recover
  std::shared_ptr<AbstractEvent> receive_event;
  cereal::PortableBinaryInputArchive inArchive(ss);
  inArchive(receive_event);
  std::shared_ptr<AbstractEvent> sp_to_handle(std::move(receive_event));

  return 0;
}

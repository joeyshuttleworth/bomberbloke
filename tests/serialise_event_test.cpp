#include <iostream>
#include <fstream>
#include <filesystem>

#include "cereal/archives/portable_binary.hpp"
#include "NetServer.hpp"
#include "engine.hpp"
#include "SyncEvent.hpp"
#include "QueryEvent.hpp"
#include "bomberbloke.h"

void new_game(std::string)
{
  return;
}

void
gameUpdate()
{
  return;
}

int test_query_event()
{
  // Generate a query event, serialise it, the deserialise it

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

  std::filesystem::create_directory("test_output");
  std::filesystem::path out_path{"test_output/query_event.bin"};
  std::ofstream fout;

  fout.open(out_path.string(), std::ios::out | std::ios::binary);
  fout.write(ss.str().data(), ss.str().size());
  fout.close();

  return 0;
}

int test_sync_event()
{
  // Generate a bomberbloke scene and a corresponding sync event. Then serialise it, the deserialise it

  _draw = false;

  SDL_Init(SDL_INIT_EVERYTHING);
  init_engine(false);
  // server_loop(0000, "xxx", false);

  NetServer net_server{};
  unsigned int player_id = 227;
  const std::string username = "big_beef";
  _pScene = std::make_shared<BomberBlokeScene>(10, 10);
  auto player = std::make_shared<NetworkPlayer>(username, player_id);
  net_server.addPlayer(player);

  player_id = player->getId();

  std::shared_ptr<AbstractEvent> s_event(new SyncEvent(player_id));
  std::stringstream ss( std::ios::in | std::ios::out | std::ios::binary );
  {
    cereal::PortableBinaryOutputArchive outputArchive(ss);
    outputArchive(s_event);
  }

  std::filesystem::path out_path{"test_output/example_sync_event.bin"};
  std::ofstream fout;
  std::filesystem::create_directory("test_output");
  fout.open(out_path.string(), std::ios::out | std::ios::binary);
  fout.write(ss.str().data(), ss.str().size());
  fout.close();

  // Recover
  std::shared_ptr<AbstractEvent> receive_event;
  cereal::PortableBinaryInputArchive inArchive(ss);
  inArchive(receive_event);
  std::shared_ptr<AbstractEvent> sp_to_handle(std::move(receive_event));

  _halt = true;
  SDL_Delay(1000);
  SDL_Quit();
  return 0;
}

int main(int, char**){
  int ret1 = test_query_event();
  int ret2 = test_sync_event();

  return (ret1 + ret2) > 0;
}

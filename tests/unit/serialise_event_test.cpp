#include <iostream>
#include <fstream>
#include <filesystem>

#include "cereal/archives/portable_binary.hpp"
#include "NetServer.hpp"
#include "engine.hpp"
#include "SyncEvent.hpp"
#include "QueryEvent.hpp"
#include "bomberbloke.h"
#include <catch2/catch_test_macros.hpp>

void new_game(std::string)
{
  return;
}

void
gameUpdate()
{
  return;
}

TEST_CASE("Can make/deserialise QueryEvent", "[engine]")
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
}

TEST_CASE("Can make/deserialise SyncEvent", "[engine]")
{
  // Generate a bomberbloke scene and a corresponding sync event. Then serialise it, the deserialise it

  _draw = false;
  _server = true;

  SDL_Init(SDL_INIT_EVERYTHING);
  init_engine(true);

  NetServer net_server{};
  int player_id = 0;

  const std::vector<std::string> usernames = {"big_beef", "big_baz", "little_john", "maid_marian", "allan_a_dale_77", "nottz_sherrif_xX", "Frair_Tuck", "Will Scarlet", "merry_man_67"};
  _pScene = std::make_shared<BomberBlokeScene>(25, 25);

  for(auto i : usernames){
	std::string username = i;
  	auto player = std::make_shared<NetworkPlayer>(username, player_id);
    player_id++;
  	net_server.addPlayer(player);
  }

  sleep(1);
  new_game("");

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
}


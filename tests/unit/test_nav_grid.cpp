#include <iostream>
#include <fstream>
#include <filesystem>

#include "engine.hpp"
#include "bomberbloke.h"
#include "NavGrid.hpp"
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


TEST_CASE("Can NavGrid", "[engine]")
{
  _draw = false;
  _server = true;

  // For now, this we need to init SDL
  SDL_Init(SDL_INIT_EVERYTHING);
  init_engine(true);
  int player_id = 0;

  const std::vector<std::string> usernames = {"big_beef", "big_baz", "little_john", "maid_marian", "allan_a_dale_77", "nottz_sherrif_xX", "Frair_Tuck", "Will Scarlet", "merry_man_67"};
  _pScene = std::make_shared<BomberBlokeScene>(25, 25);

  for(auto i : usernames){
    std::string username = i;
  	auto player = std::make_shared<NetworkPlayer>(username, player_id);
    player_id++;
    _player_list.push_back(player);
  }

  /* Initiate scene */
  new_game("");

  /* Generate NavGrid */
  std::vector<std::type_index> blocking_types = {
    typeid(bloke),
    typeid(bomb),
    typeid(WoodenCrate),
    typeid(StoneBlock)
  };

  NavGrid nav_grid(blocking_types, _pScene);

  nav_grid.ComputeGrid();

  assert(nav_grid.mNodes.size() > 0);

  return;
}

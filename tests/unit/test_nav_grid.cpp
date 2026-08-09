#include <iostream>
#include <fstream>
#include <filesystem>

#include "engine.hpp"
#include "bomberbloke.h"
#include "NavGrid.hpp"
#include "NetworkPlayer.hpp"
#include <catch2/catch_test_macros.hpp>


/* For generating NavGrid - list of blocks that block movement */
std::vector<std::type_index> blocking_types = {
  typeid(bloke),
  typeid(bomb),
  typeid(WoodenCrate),
  typeid(StoneBlock)
};


void
new_game(IOSystem&, std::string)
{
  return;
}


void
gameUpdate()
{
  return;
}

TEST_CASE("Can create NavGrid", "[engine]")
{
  _draw = false;
  _server = true;

  // Engine relies on SDL so init it here
  SDL_Init(SDL_INIT_EVERYTHING);

  init_engine(_fallback_IO_system, true);
  int player_id = 0;

  const std::vector<std::string> usernames = {"big_beef", "big_baz", "little_john", "maid_marian", "allan_a_dale_77", "nottz_sherrif_xX", "Frair_Tuck", "Will Scarlet", "merry_man_67"};
  _pScene = std::make_shared<BomberBlokeScene>(_fallback_IO_system, 25, 25);

  for(auto i : usernames){
    std::string username = i;
    std::shared_ptr<AbstractPlayer> player = std::make_shared<NetworkPlayer>(username, player_id);
    player_id++;
    _player_list.push_back(player);
  }

  /* Initiate scene */
  new_game(_fallback_IO_system, "");

  NavGrid nav_grid(blocking_types, _pScene);
  nav_grid.computeGrid();
  assert(nav_grid.mNodes.size() > 0);

  return;
}

TEST_CASE("Full NavGrid for empty level", "[engine]")
{
  _pScene = std::make_shared<BomberBlokeScene>(_fallback_IO_system, 10, 10);
  _pScene->mActors.clear();
  NavGrid nav_grid(blocking_types, _pScene);
  nav_grid.computeGrid();
  assert(nav_grid.mNodes.size() == _pScene->mDimension[0] * _pScene->mDimension[1]);
  assert(nav_grid.getNeighbours(ivector{0, 0}, false).size() == 2);

  assert(nav_grid.getNeighbours(ivector{0, 0}, true).size() == 3);
  assert(nav_grid.getNeighbours(ivector{1, 1}, false).size() == 4);
  assert(nav_grid.getNeighbours(ivector{1, 1}, true).size() == 8);
}

TEST_CASE("Empty NavGrid for full level", "[engine]")
{
  _pScene = std::make_shared<BomberBlokeScene>(_fallback_IO_system, 10, 10);
  _pScene->mActors.clear();

  /* Create WoodenCrates at each square */
  for(int i=0; i < 10; i++){
    for(int j=0; j < 10; j++){
      auto crate = std::make_shared<WoodenCrate>(_pScene.get(), i, j);
      _pScene->addActor(crate);
    }
  }

  NavGrid nav_grid(blocking_types, _pScene);
  nav_grid.computeGrid();

  std::cout << "Number of elements in grid: " << nav_grid.mNodes.size() << std::endl;
  assert(nav_grid.mNodes.size() == 0);

  /* Check that neighbours function doesn't return any neighbours */
  REQUIRE_THROWS_AS(nav_grid.getNeighbours(ivector{0, 0}), std::invalid_argument);
}

TEST_CASE("Components of disconnected level", "[engine]")
{
  _pScene = std::make_shared<BomberBlokeScene>(_fallback_IO_system, 10, 10);
  _pScene->mActors.clear();

  /* Create WoodenCrates at each square */
  int j = 5;
  for(int i=0; i < 10; i++){
    auto crate = std::make_shared<WoodenCrate>(_pScene.get(), i, j);
    _pScene->addActor(crate);
  }

  NavGrid nav_grid(blocking_types, _pScene);
  nav_grid.computeGrid();

  assert(nav_grid.mNodes.size() == 10 * 10 - 10);

  auto connected_comps = nav_grid.getConnectedComponents();

  assert(connected_comps.size()==2);
  assert(connected_comps[0].size() == 5*10);
  assert(connected_comps[1].size() == 4*10);
}


TEST_CASE("Get A* route", "[engine]")
{
  _pScene = std::make_shared<BomberBlokeScene>(_fallback_IO_system, 10, 10);
  _pScene->mActors.clear();

  /* Create WoodenCrates at each square */
  int j = 5;
  for(int i=0; i < 10; i++){
    auto crate = std::make_shared<WoodenCrate>(_pScene.get(), i, j);
    _pScene->addActor(crate);
  }
  NavGrid nav_grid(blocking_types, _pScene);
  nav_grid.computeGrid();

  assert(nav_grid.mNodes.size() == 10 * 10 - 10);

  {
    ivector start = {0, 0};
    ivector goal = {1, 0};
    auto route = nav_grid.findRoute(start, goal);
    assert(route.size() == 2);
  }

  {
    ivector start = {0, 0};
    ivector goal = {9, 0};
    auto route = nav_grid.findRoute(start, goal);
    assert(route.size() == 10);
  }

  {
    ivector start = {0, 0};
    ivector goal = {9, 9};
    auto route = nav_grid.findRoute(start, goal);
    assert(route.size() == 0);
  }

  {
    ivector start = {0, 0};
    ivector goal = {100, 100};
    auto route = nav_grid.findRoute(start, goal);
    assert(route.size() == 0);
  }
}



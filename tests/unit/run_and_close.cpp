#include "BomberBlokeScene.hpp"
#include "bloke.hpp"
#include "bomb.hpp"
#include "bomberbloke.h"
#include "LocalPlayer.hpp"
#include <SDL2/SDL.h>
#include <catch2/catch_test_macros.hpp>

extern int _log_message_level;

TEST_CASE("Can make a game scene", "[engine]")
{
  _draw = false;
  _log_message_level = CRITICAL;

  REQUIRE(SDL_Init(SDL_INIT_EVERYTHING) == 0);
  init_engine(false);

  const std::string username = "big_beef";
  _local_player_list.push_back(LocalPlayer(username));

  _pScene = std::make_shared<BomberBlokeScene>(nullptr, 10, 10);
  REQUIRE(_pScene != nullptr);
  REQUIRE(_pScene->mDimension[0] == 10);
  REQUIRE(_pScene->mDimension[1] == 10);

  _halt = true;
  client_loop();

  SDL_Quit();
}

void
gameUpdate()
{
  return;
}

void
new_game(std::string)
{
  return;
}

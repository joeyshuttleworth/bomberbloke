#include "ServerInfo.hpp"
#include "ServerInfoEvent.hpp"
#include "bomberbloke.h"
#include <SDL2/SDL.h>
#include <array>
#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <memory>
#include <random>
#include <stdio.h>

int _default_bomb_time = DEFAULT_BOMB_TIMER;
double _bloke_size[2] = { DEFAULT_BLOKE_SIZE, DEFAULT_BLOKE_SIZE };
std::vector<int> _spawn_points = { 5, 5 };
int colours[50][3];

int
main(int argc, char **argv)
{
  bool debug = false;
  if(argc > 1 && strcmp(argv[1], "--debug") == 0) {
    debug = true;
    log_message(INFO, "DEBUG Mode: On");
  }

  log_message(INFO, "Bomberbloke server starting...");

  init_engine();

  _pScene = std::make_shared<BomberBlokeScene>(10, 10);

  server_loop(debug);
  SDL_Quit();
  SDL_Delay(1000);
  return 0;
}

void new_game(std::string)
{
  /* Lock _scene_mutex to protect _pScene from other threads */
  const std::lock_guard<std::mutex> lock(_scene_mutex);

  /* First set reset everyone's powerups */
  for (auto i = _player_list.begin(); i != _player_list.end(); i++) {
    if (!(*i)->getPlayerProperties())
      (*i)->resetPlayerProperties(std::make_shared<GamePlayerProperties>());
    (*i)->resetPlayerProperties();
  }

  _pScene = std::make_shared<BomberBlokeScene>(10, 10);

  /* Reset everyone's powerups */
  for (auto i = _player_list.begin(); i != _player_list.end(); i++) {
    if (!(*i)->getPlayerProperties())
      (*i)->resetPlayerProperties(std::make_shared<GamePlayerProperties>());
    (*i)->resetPlayerProperties();
  }
}

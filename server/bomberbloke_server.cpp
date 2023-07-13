#include "ServerInfo.hpp"
#include "ServerInfoEvent.hpp"
#include "bomberbloke.h"
#include <SDL2/SDL.h>
#include <array>
#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <getopt.h>
#include <memory>
#include <random>
#include <stdio.h>

double _bloke_size[2] = { DEFAULT_BLOKE_SIZE, DEFAULT_BLOKE_SIZE };
std::vector<int> _spawn_points = { 5, 5 };
int colours[50][3];

int _default_bomb_time = DEFAULT_BOMB_TIMER;
bool _debug = false;
short _port = 8888;
std::string _masterServerAddress = "";

void
help()
{
  std::cout << std::endl 
            << "--debug:              Enable debug mode\n"
               "--port <n>:           Port to start server on\n"
               "--masterServer <url>: URL of the master server\n"
               "--help:               Show help\n";
  exit(1);
}

int
main(int argc, char** argv)
{
  int iarg = 0;
  const char* const short_opts = "dp:h";
  const option long_opts[] = {
    { "debug", no_argument, nullptr, 'd' },
    { "port", required_argument, nullptr, 'p' },
    { "masterServer", required_argument, nullptr, 'm' },
    { "help", no_argument, nullptr, 'h' },
    { nullptr, no_argument, nullptr, 0 }
  };

  while (iarg != -1) {
    iarg = getopt_long(argc, argv, short_opts, long_opts, nullptr);

    switch (iarg) {
      case 'd':
        _debug = true;
        break;
      case 'p':
        _port = short(std::stoi(optarg));
        break;
      case 'm':
        _masterServerAddress = std::string(optarg);
        break;
      case 'h':
      case '?': // unrecognised
        help();
        break;
    }
  }

  log_message(INFO, "Bomberbloke server starting...");

  init_engine(true);

  _pScene = std::make_shared<BomberBlokeScene>(10, 10);

  server_loop(_port, _masterServerAddress, _debug);
  SDL_Quit();
  SDL_Delay(1000);
  return 0;
}

void
new_game(std::string)
{
  /* Lock _scene_mutex to protect _pScene from other threads */
  const std::lock_guard<std::mutex> lock(_scene_mutex);

  _pScene = std::make_shared<BomberBlokeScene>(10, 10);

  /* Reset everyone's powerups */
  for (auto i = _player_list.begin(); i != _player_list.end(); i++) {
    if (!(*i)->getPlayerProperties())
      (*i)->resetPlayerProperties(std::make_shared<GamePlayerProperties>());
    (*i)->resetPlayerProperties();
  }
}

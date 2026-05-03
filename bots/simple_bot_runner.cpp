#include <random>
#include "SimpleBot.hpp"
#include "LocalPlayer.hpp"
#include "bomberbloke.h"
#include "BomberBlokeScene.hpp"
#include "Explosion.hpp"
#include "MainMenuScene.hpp"
#include "bloke.hpp"
#include "bomb.hpp"
#include "IOSystem.hpp"
#include "SimpleBot.hpp"
#include "scene.hpp"
#include "DummyGraphicsManager.hpp"
#include "DummySoundManager.hpp"
#include <SDL2/SDL.h>
#include <getopt.h>
#include <network/NetClient.hpp>


/* Register our actors with cereal */
CEREAL_REGISTER_DYNAMIC_INIT(actor)

std::unique_ptr<SimpleBot> _simple_bot;

int
main(int argc, char** argv)
{
  std::string user_name = "simple_bot";
  std::string server_address = "127.0.0.1:8888";
  int iarg = 0;
  const char* const short_opts = "u:s:";
  const option long_opts[] = { { "username", required_argument, nullptr, 'u' },
                               { "server", required_argument, nullptr, 's' },
                               { nullptr, no_argument, nullptr, 0 } };

  _draw = false;
  _server = false;

  while (iarg != -1) {
    iarg = getopt_long(argc, argv, short_opts, long_opts, nullptr);
    switch (iarg) {
    case 'u':
      user_name = std::string(optarg);
      break;
    case 's':
      server_address = std::string(optarg);
      break;
    }
  }

  _local_player_list.push_back(LocalPlayer(user_name));
  _nickname = user_name;

  SDL_Init(SDL_INIT_EVERYTHING);
  init_engine(_fallback_IO_system, false);

  IOSystem io_system_context;
  _pScene = std::make_shared<BomberBlokeScene>(io_system_context);

  // Attempt join
  std::vector<std::string> commands = { "colour FFFFFFFF" };

  _simple_bot = std::unique_ptr<SimpleBot>(new SimpleBot(_nickname, _pScene, nullptr,
                                                         std::unique_ptr<NetClient>(new NetClient())));

  if (_simple_bot->mpNetClient->joinBlokeServer(server_address, user_name, commands)) {
    std::vector<std::string> commands = { "colour FFFFFFFF" };
    bot_loop();
  }

  return 0;
}

// Required by the engine
void
gameUpdate()
{
  return;
}

// Required by the engine
void
new_game(IOSystem&, std::string)
{
  _simple_bot->handleReset();
  return;
}


void bot_entry() {
  /* Lock _scene_mutex to protect _pScene from other threads */
  LOCK_GUARD(_scene_mutex);

  _simple_bot->mpNetClient->pollServer();

  if (_pScene) {
    _pScene->update();

    if(!_simple_bot->isConnectedToScene()){
      _simple_bot->connectScene(_pScene);
    }

    _simple_bot->update(_tick);
  }

  if(!_pScene){
    log_message(WARNING, "No to pointer scene");
  }

  _tick++;
}


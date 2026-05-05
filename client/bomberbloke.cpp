#include "bomberbloke.h"
#include "LocalPlayer.hpp"
#include "BomberBlokeScene.hpp"
#include "Explosion.hpp"
#include "MainMenuScene.hpp"
#include "bloke.hpp"
#include "bomb.hpp"
#include "SDLGraphicsManager.hpp"
#include "SDLSoundManager.hpp"
#include "SDLInputManager.hpp"
#include <getopt.h>
#include <network/NetClient.hpp>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

const bool EXPLOSION_INTRO = false;

/* Register our actors with cereal */
CEREAL_REGISTER_DYNAMIC_INIT(actor)

int
main(int argc, char** argv)
{
  std::string userName = "bloke";
  std::string serverAddress = "";
  bool autoConnect = false;

  int iarg = 0;
  const char* const short_opts = "u:s:";
  const option long_opts[] = { { "username", required_argument, nullptr, 'u' },
                               { "server", required_argument, nullptr, 's' },
                               { nullptr, no_argument, nullptr, 0 } };

  while (iarg != -1) {
    iarg = getopt_long(argc, argv, short_opts, long_opts, nullptr);
    switch (iarg) {
      case 'u':
        userName = std::string(optarg);
        break;
      case 's':
        serverAddress = std::string(optarg);
        break;
    }
  }

  if (userName.length() > 0 && serverAddress.length() > 0) {
    autoConnect = true;
  }

  _default_bindings = { { KEY_W, "up" },
                        { KEY_S, "down" },
                        { KEY_A, "left" },
                        { KEY_D, "right" },
                        { KEY_J, "powerup" },
                        { KEY_K, "bomb" },
                        { KEY_P, "pause" },
                        { KEY_ESCAPE, "toggle_pause" },
                        { KEY_MINUS, "zoom *.8" },
                        { KEY_EQUALS, "zoom *1.25" },
                        { KEY_F, "zoom follow" },
                        { KEY_0, "zoom all" } };

  _local_player_list.push_back(LocalPlayer(userName));
  _nickname = userName;

  IOSystem io_system_context(
                             std::make_unique<SDLGraphicsManager>(),
                             std::make_unique<SDLSoundManager>(),
                             std::make_unique<SDLInputManager>()
                             );

  client_init(io_system_context);

  _pScene = std::make_shared<MainMenuScene>(io_system_context, 15, 15);

  if (EXPLOSION_INTRO) {
    for (unsigned int i = 0; i < 10; i++) {
      for (unsigned int j = 0; j < 10; j++)
        _pScene->mParticles.push_back(std::shared_ptr<Explosion>(
                                                                 new Explosion(_fallback_IO_system.getGraphicsManager(),
                                                                               _fallback_IO_system.getSoundManager(),
                                                                               i, j, 1, 1, 60 + i + 2 * j, 600 - 2 * i - j, 0)));
    }

    ISoundManager& sound_manager = io_system_context.getSoundManager();
    // Play intro music
    std::shared_ptr<Sound> pIntroSound =
      sound_manager.createSound("explosion_intro");
    sound_manager.playSound(pIntroSound.get());
    pIntroSound->setGroup(SOUND_FX);
  }

  if (autoConnect) {
    std::vector<std::string> commands = { "colour FFFFFFFF" };

    if (_net_client->joinBlokeServer(serverAddress, userName, commands)) {
      _pNewScene = std::make_shared<BomberBlokeScene>(io_system_context);
    }
  }

#ifndef __EMSCRIPTEN__
  client_loop(io_system_context);
#else
  emscripten_set_main_loop([&]()->void {client_loop(io_system_context)}, 0, true);
#endif

  return 0;
}

void
gameUpdate()
{
  return;
}

void
new_game(IOSystem&, std::string)
{
  return;
}

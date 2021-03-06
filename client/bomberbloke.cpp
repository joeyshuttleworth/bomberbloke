#include "bomberbloke.h"
#include <SDL2/SDL.h>
#include "bloke.hpp"
#include "bomb.hpp"
#include <network/NetClient.hpp>
#include "Explosion.hpp"
#include "MainMenuScene.hpp"

const bool EXPLOSION_INTRO = false;

/* Register our actors with cereal */
CEREAL_REGISTER_DYNAMIC_INIT(actor)

int main (){
  _default_bindings =
          {{SDL_SCANCODE_W, "up"},
           {SDL_SCANCODE_S, "down"},
           {SDL_SCANCODE_A, "left"},
           {SDL_SCANCODE_D, "right"},
           {SDL_SCANCODE_J, "powerup"},
           {SDL_SCANCODE_K, "bomb"},
           {SDL_SCANCODE_P, "pause"}
          };

  _local_player_list.push_back(LocalPlayer(std::string("big_beef")));

  SDL_Init(SDL_INIT_EVERYTHING);
  init_engine();

  _pScene = std::make_shared<MainMenuScene>(15, 15);

  if (EXPLOSION_INTRO) {
    for(unsigned int i = 0; i < 10; i++) {
      for(unsigned int j = 0; j < 10; j++)
        _pScene->mParticleList.push_back(std::shared_ptr<Explosion>(new Explosion(i, j, 1, 1, 60 + i + 2*j, 600 - 2*i - j, 0)));
    }

    // Play intro music
    std::shared_ptr<Sound> pIntroSound = soundManager.createSound("explosion_intro");
    soundManager.playSound(pIntroSound);
    pIntroSound->mGroup = SOUND_FX;
  }

  client_loop();

  return 0;
}

void gameUpdate(){
  return;
}

void new_game(std::string){
  return;
}

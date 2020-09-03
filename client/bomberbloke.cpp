#include "bomberbloke.h"
#include <SDL2/SDL.h>
#include "bloke.hpp"
#include "bomb.hpp"
#include <network/NetClient.hpp>
#include "Explosion.hpp"
#include "TextButton.hpp"
#include "TextHudElement.hpp"
#include "SpriteHudElement.hpp"
#include "InputField.hpp"

/* Register our actors with cereal */
CEREAL_REGISTER_DYNAMIC_INIT(actors)

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

  SDL_StartTextInput();
  _pScene = std::make_shared<BomberBlokeScene>(10,10);

  for(unsigned int i = 0; i < 10; i++)
    for(unsigned int j = 0; j < 10; j++){
      _pScene->mParticleList.push_back(std::shared_ptr<Explosion>(new Explosion(i, j, 1, 1, 60 + i + 2*j, 600 - 2*i - j)));
    }

  // Play intro music
  std::shared_ptr<Sound> pIntroSound = soundManager.createSound("explosion_intro");
  soundManager.playSound(pIntroSound);
  client_loop();

  return 0;
}

void gameUpdate(){
  return;
}

void new_game(std::string){
  return;
}

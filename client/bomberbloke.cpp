#include "bomberbloke.h"
#include <SDL2/SDL.h>
#include "bloke.hpp"
#include "bomb.hpp"
#include <network/NetClient.hpp>
#include "Explosion.hpp"
#include "ClickableHudElement.hpp"

/* Register our actors with cereal */
CEREAL_REGISTER_DYNAMIC_INIT(actors)

void hudTestFn1() {
    std::cout << "clicked centre button" << std::endl;
}

void hudTestFn2() {
    std::cout << "clicked left button" << std::endl;
    handle_system_command(split_to_tokens("nickname dave1"));
    handle_system_command(split_to_tokens("open 127.0.0.1"));
}

void hudTestFn3() {
    std::cout << "clicked right button" << std::endl;
    handle_system_command(split_to_tokens("nickname dave2"));
    handle_system_command(split_to_tokens("open 127.0.0.1"));
}

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

  SDL_Init(SDL_INIT_EVERYTHING);
  init_engine();
  _local_player_list.push_back(LocalPlayer(std::string("big_beef")));

  std::shared_ptr<bloke> b1(new bloke(1, 2, true));

  for(unsigned int i = 0; i < 10; i++)
    for(unsigned int j = 0; j < 10; j++){
      _pScene->mParticleList.push_back(std::shared_ptr<Explosion>(new Explosion(i, j, 1, 1, 60 + i + 2*j, 600 - 2*i - j)));
    }

  // Play intro music
  std::shared_ptr<Sound> pIntroSound = soundManager.createSound("explosion_intro");
  soundManager.playSound(pIntroSound);
  
  // HUD elements for testing
  std::shared_ptr<AbstractHudElement> hudElement1(new ClickableHudElement(5, 5, 100, 50, hudTestFn1, ALIGN_CENTER, ALIGN_BOTTOM));
  _pScene->mHudElements.push_back(hudElement1);
  std::shared_ptr<AbstractHudElement> hudElement2(new ClickableHudElement(5, 5, 100, 50, hudTestFn2, ALIGN_LEFT, ALIGN_BOTTOM));
  _pScene->mHudElements.push_back(hudElement2);
  std::shared_ptr<AbstractHudElement> hudElement3(new ClickableHudElement(5, 5, 100, 50, hudTestFn3, ALIGN_RIGHT, ALIGN_BOTTOM));
  _pScene->mHudElements.push_back(hudElement3);

  // TODO: get rid of this bodge that allows for the test HUD elements to draw 
  _pCamera->onResize();
  
  client_loop();

  return 0;
}

void gameUpdate(){
  return;
}

void new_game(std::string){
  return;
}










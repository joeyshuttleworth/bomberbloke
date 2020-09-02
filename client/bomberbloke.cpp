#include "bomberbloke.h"
#include <SDL2/SDL.h>
#include "bloke.hpp"
#include "bomb.hpp"
#include <network/NetClient.hpp>
#include "Explosion.hpp"
#include "TextButton.hpp"
#include "InputField.hpp"
#include "TextHudElement.hpp"
#include "SpriteHudElement.hpp"


/* Register our actors with cereal */
CEREAL_REGISTER_DYNAMIC_INIT(actors)

void hudTestFn1() {
    handle_system_command(split_to_tokens("nickname dave1"));
}

void hudTestFn2() {
    handle_system_command(split_to_tokens("nickname dave2"));
}

void hudTestFnJoin() {
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
  std::shared_ptr<Text> pTextTitle = textManager.createText("Aileron-Black", "BLOKE/ENGINE");
  pTextTitle->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
  pTextTitle->setTextColour({255, 255, 255});
  pTextTitle->setTextScale(2.);
  std::shared_ptr<TextHudElement> hudElementTitle = std::make_shared<TextHudElement>(pTextTitle, 5, 5, 400, 50, ALIGN_CENTER);
  _pScene->mHudElements.push_back(hudElementTitle);

  std::shared_ptr<Text> pText1 = textManager.createText("Aileron-Black", "DAVE1");
  pText1->setTextAlignment(TEXT_ALIGN_LEFT, TEXT_ALIGN_CENTER);
  pText1->setTextColour({255, 255, 255});
  pText1->setTextScale(1.5);
  std::shared_ptr<TextButton> hudElement1 = std::make_shared<TextButton>(pText1, 9, 71, 200, 30, hudTestFn1, ALIGN_LEFT, ALIGN_BOTTOM);
  hudElement1->setMouseOverColour({200, 200, 200});
  hudElement1->setOnClickOffset(1, 2);
  _pScene->mHudElements.push_back(hudElement1);

  std::shared_ptr<Text> pText2 = textManager.createText("Aileron-Black", "DAVE2");
  pText2->setTextAlignment(TEXT_ALIGN_LEFT, TEXT_ALIGN_CENTER);
  pText2->setTextColour({255, 255, 255});
  pText2->setTextScale(1.5);
  std::shared_ptr<TextButton> hudElement2 = std::make_shared<TextButton>(pText2, 9, 40, 200, 30, hudTestFn2, ALIGN_LEFT, ALIGN_BOTTOM);
  hudElement2->setMouseOverColour({200, 200, 200});
  hudElement2->setOnClickOffset(1, 2);
  _pScene->mHudElements.push_back(hudElement2);

  std::shared_ptr<Text> pTextJoin = textManager.createText("Aileron-Black", "JOIN LOCAL");
  pTextJoin->setTextAlignment(TEXT_ALIGN_LEFT, TEXT_ALIGN_CENTER);
  pTextJoin->setTextColour({255, 255, 255});
  pTextJoin->setTextScale(1.5);
  std::shared_ptr<TextButton> hudElementJoin = std::make_shared<TextButton>(pTextJoin, 9, 9, 200, 30, hudTestFnJoin, ALIGN_LEFT, ALIGN_BOTTOM);
  hudElementJoin->setMouseOverColour({200, 200, 200});
  hudElementJoin->setOnClickOffset(0, 3);
  _pScene->mHudElements.push_back(hudElementJoin);


  std::shared_ptr<Text> pTextInput = textManager.createText("Aileron-Black", "Name");
  pTextInput->setTextAlignment(TEXT_ALIGN_LEFT, TEXT_ALIGN_CENTER);
  pTextInput->setTextColour({255, 255, 255});
  pTextInput->setTextScale(1.5);
  std::shared_ptr<InputField> hudElementInput = std::make_shared<InputField>(pTextInput, 9, 120, 400, 30, nullptr, ALIGN_LEFT, ALIGN_BOTTOM);
  hudElementInput->setOnClickOffset(0, 3);
  _pScene->mHudElements.push_back(hudElementInput);

  std::shared_ptr<Text> pTextInput2 = textManager.createText("Aileron-Black", "SERVER IP");
  pTextInput2->setTextAlignment(TEXT_ALIGN_LEFT, TEXT_ALIGN_CENTER);
  pTextInput2->setTextColour({255, 255, 255});
  pTextInput2->setTextScale(1.5);
  std::shared_ptr<InputField> hudElementInput2 = std::make_shared<InputField>(pTextInput2, 9, 150, 400, 30, nullptr, ALIGN_LEFT, ALIGN_BOTTOM);
  hudElementInput2->setOnClickOffset(0, 3);
  _pScene->mHudElements.push_back(hudElementInput2);


  // Speed HUD demo
  for (int i = 0; i < 4; i++) {
      std::shared_ptr<SpriteHudElement> hudElement = std::make_shared<SpriteHudElement>("lightning.png", 9 + i * 34, 9, 32, 32);
      _pScene->mHudElements.push_back(hudElement);
  }
  
  // Power HUD demo
  for (int i = 0; i < 3; i++) {
      std::shared_ptr<SpriteHudElement> hudElement = std::make_shared<SpriteHudElement>("flames.png", 9 + i * 34, 9, 32, 32, ALIGN_RIGHT);
      _pScene->mHudElements.push_back(hudElement);
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

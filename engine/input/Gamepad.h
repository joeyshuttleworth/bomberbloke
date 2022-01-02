//
// Created by dave on 02/01/2022.
//

#ifndef BOMBERBLOKE_GAMEPAD_H
#define BOMBERBLOKE_GAMEPAD_H

#include <SDL_joystick.h>
#include <SDL_events.h>
#include <SDL_haptic.h>

class Gamepad
{
public:
  void cleanup();
  std::__cxx11::basic_string<char> event(const SDL_Event&);
  bool isConnected;
  bool hasRumble;
  void rumble(float strength, int ms);

private:
  SDL_GameController *mController;
  SDL_Haptic* mControllerHaptic = NULL;
  void connect();
  const int JOYSTICK_DEAD_ZONE = 8000;
};

#endif // BOMBERBLOKE_GAMEPAD_H

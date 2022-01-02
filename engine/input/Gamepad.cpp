//
// Created by dave on 02/01/2022.
//
#include "Gamepad.h"

#include <iostream>


void
Gamepad::connect()
{
  // we could theoretically enumerate multiple, but
  // local multiplayer support must be added first
  if (SDL_NumJoysticks() > 0) {
    std::cout << "Controller connected\n";
    mController = SDL_GameControllerOpen(0); // return first joystick identifier
    if (mController == nullptr){
        std::cout << "Failed to connect controller\n";
      }
    else {
        isConnected = true;
        mControllerHaptic = SDL_HapticOpenFromJoystick( SDL_GameControllerGetJoystick(mController) );
        if( SDL_HapticRumbleInit( mControllerHaptic ) < 0 ){
          std::cout << "Failed to find controller haptics\n";
        }
        else {
          hasRumble = true;
          std::cout << "Haptics connected\n";

        }
      }
  }
}

/**
 * Rumble gamepad for selected duration
 *
 * @param strength From 0-1.
 * @param ms Time in ms to play rumble for
 * @return void
 */
void Gamepad::rumble(float strength, int ms){
  if( SDL_HapticRumblePlay( mControllerHaptic, strength, ms ) != 0 ){
    std::cout << "Error playing rumble";
  }
}

void Gamepad::cleanup(){
SDL_GameControllerClose(mController);
mController = nullptr;
}

std::string
Gamepad::event(const SDL_Event& event)
{
  switch (event.type) {
    case SDL_CONTROLLERAXISMOTION:
      // Motion on controller 0
      if (event.jaxis.which == 0) {
        // X axis motion
        if (event.jaxis.axis == 0) {
          // Left of dead zone
          if (event.jaxis.value < -JOYSTICK_DEAD_ZONE) {
            return "left";
          }
          // Right of dead zone
          else if (event.jaxis.value > JOYSTICK_DEAD_ZONE) {
            return "right";
          }
        }
        // Y axis motion
        else if (event.jaxis.axis == 1) {
          // Below of dead zone
          if (event.jaxis.value < -JOYSTICK_DEAD_ZONE) {
            return "-down";
          }
          // Above of dead zone
          else if (event.jaxis.value > JOYSTICK_DEAD_ZONE) {
            return "+up";
          }

        }
      }
  std::cout << "Axis motion\n";
  break;
  case SDL_CONTROLLERBUTTONUP:
  case SDL_CONTROLLERBUTTONDOWN:
    std::cout << "Buttons pressed \n";
    break;
  case SDL_CONTROLLERDEVICEADDED: {
    connect();
    std::cout << "Added";

    break;
  }
  case SDL_CONTROLLERDEVICEREMOVED: {
    std::cout << "Removed";
    break;
  }
}
}

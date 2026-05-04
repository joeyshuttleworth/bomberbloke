#include "SDLInputEvent.hpp"
#include <SDL2/SDL_events.h>


unsigned int SDLInputEvent::getKey() const
{
  if(mInputType != IEVENT_KEYDOWN || mInputType != IEVENT_KEYUP)
    {
      return event.key.keysym.scancode;
    }
  return 0;
}


std::array<unsigned int, 2> SDLInputEvent::getMouseLocation() const
{
  std::array<unsigned int, 2> loc = {0, 0};
  switch(mEvent.type)
    {
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
      loc = {mEvent.button.x, mEvent.button.y};
      break;
    case SDL_MOUSEMOTION:
      loc = {mEvent.motion.x, mEvent.motion.y};
      break;
    }

  return;
}

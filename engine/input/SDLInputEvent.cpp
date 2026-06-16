#include "SDLInputEvent.hpp"
#include <SDL2/SDL_events.h>

const std::array<int, 2> SDLInputEvent::getMouseLocation() const
{
  switch(mSDLEvent.type)
    {
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
      return {mSDLEvent.button.x, mSDLEvent.button.y};
    case SDL_MOUSEMOTION:
      return {mSDLEvent.motion.x, mSDLEvent.motion.y};
    default:
      break;
    }

  return {0, 0};
}


InputKey SDLInputEvent::getPressedKey() const
{

  switch(getInputType()){
  case IEVENT_KEYDOWN:
  case IEVENT_KEYUP:
    break;
  default:
    return KEY_UNKNOWN;
  }

  unsigned int scancode = (unsigned int) mSDLEvent.key.keysym.scancode;
  return (InputKey) scancode;
}

static InputEventType getEventType(SDL_Event& event)
{
  InputEventType e_type;

  switch(event.type){
  case SDL_QUIT:
    e_type = IEVENT_QUIT;
    break;
  case SDL_KEYDOWN:
    e_type = IEVENT_KEYDOWN;
    break;
  case SDL_KEYUP:
    e_type = IEVENT_KEYUP;
    break;
  case SDL_WINDOWEVENT:
    if(event.window.event == SDL_WINDOWEVENT_RESIZED)
      e_type = IEVENT_WINDOWRESIZEEVENT;
    else
      e_type = IEVENT_NONE;
    break;
  case SDL_JOYAXISMOTION:
    e_type = IEVENT_JOYAXISMOTION;
    break;
  case SDL_MOUSEBUTTONUP:
    e_type = IEVENT_MOUSEBUTTONUP;
    break;
  case SDL_MOUSEBUTTONDOWN:
    e_type = IEVENT_MOUSEBUTTONDOWN;
    break;
  case SDL_MOUSEMOTION:
    e_type = IEVENT_MOUSEMOTION;
    break;
  default:
    e_type = IEVENT_NONE;
    break;
  }
  return e_type;
}

SDLInputEvent::SDLInputEvent(SDL_Event event) :
  AbstractInputEvent(getEventType(event)),
  mSDLEvent(event)
{
}

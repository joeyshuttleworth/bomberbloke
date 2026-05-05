#ifndef SDLINPUTEVENT_HPP
#define SDLINPUTEVENT_HPP

#include "AbstractInputEvent.hpp"

#include <SDL2/SDL_events.h>
#include <array>

class scene;
class SDLInputManager;

class SDLInputEvent : public AbstractInputEvent
{
  friend SDLInputManager;
public:
  using AbstractInputEvent::AbstractInputEvent;
  using AbstractInputEvent::operator=;


  SDLInputEvent(SDL_Event);

  ~SDLInputEvent() = default;

  const std::array<int, 2> getMouseLocation() const override;

  InputKey getPressedKey() const override;

protected:
  const SDL_Event mSDLEvent;
};


#endif

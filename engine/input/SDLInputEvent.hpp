#ifndef SDLINPUTEVENT_HPP
#define SDLINPUTEVENT_HPP

#include "AbstractInputEvent.hpp"

#include <SDL2/SDL_events.h>
#include <array>

class scene;

class SDLInputEvent : public AbstractInputEvent
{

public:
  using AbstractInputEvent::AbstractInputEvent;
  using AbstractInputEvent::operator=;


  SDLInputEvent(SDL_Event);

  ~SDLInputEvent();

  unsigned int getKey() const override;
  std::array<unsigned int, 2> getMouseLocation() const override;

  InputKey getPressedKey() override;

protected:
  const SDL_Event mSDLEvent;
};


#endif

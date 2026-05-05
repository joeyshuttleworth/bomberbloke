#ifndef ABSTRACTINPUTEVENT_HPP
#define ABSTRACTINPUTEVENT_HPP

#include "InputKey.hpp"

#include <array>

enum InputEventType {
  IEVENT_NONE = 0,
  IEVENT_QUIT,
  IEVENT_KEYDOWN,
  IEVENT_KEYUP,
  IEVENT_WINDOWRESIZEEVENT,
  IEVENT_JOYAXISMOTION,
  IEVENT_MOUSEBUTTONUP,
  IEVENT_MOUSEBUTTONDOWN,
  IEVENT_MOUSEMOTION
};


class AbstractInputEvent{

public:
  AbstractInputEvent(InputEventType e_type) : mInputType(e_type){}

  ~AbstractInputEvent(){}

  AbstractInputEvent(AbstractInputEvent& other) : mInputType(other.mInputType){}

  AbstractInputEvent(AbstractInputEvent&& other) : mInputType(other.mInputType){}

  AbstractInputEvent& operator=(AbstractInputEvent& other) = delete;

  virtual const std::array<int, 2> getMouseLocation() const {return {0, 0};}

  InputEventType getInputType() const {
    return mInputType;
  }

  virtual InputKey getPressedKey() const {return KEY_UNKNOWN;}

protected:
  const InputEventType mInputType = IEVENT_NONE;
};


#endif

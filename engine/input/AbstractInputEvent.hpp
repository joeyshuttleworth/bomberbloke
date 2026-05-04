#ifndef ABSTRACTINPUTEVENT_HPP
#define ABSTRACTINPUTEVENT_HPP

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


enum InputKey{
  KEY_NONE = 0,
  KEY_ESC,
  KEY_LEFTARROW,
  KEY_RIGHTARROW,
  KEY_UPARROW,
  KEY_DOWNARROW,
  KEY_UPARROW,
  KEY_DOWNARROW,
  KEY_TAB,
  KEY_BACKSPACE,
  KEY_RETURN
}

class AbstractInputEvent{

public:
  AbstractInputEvent(InputEventType e_type) : mInputType(e_type){}

  ~AbstractInputEvent(){}

  AbstractInputEvent(AbstractInputEvent& other) : mInputType(other.mInputType){}

  AbstractInputEvent(AbstractInputEvent&& other) : mInputType(other.mInputType){}

  AbstractInputEvent& operator=(AbstractInputEvent& other) = delete;

  virtual unsigned int getKey() const {return 0;}

  virtual std::array<unsigned int, 2> getMouseLocation() const {return {0, 0};}

  InputEventType getInputType() const {
    return mInputType;
  }

protected:
  const InputEventType mInputType = IEVENT_NONE;
};


#endif

#ifndef IINPUTMANAGER_HPP
#define IINPUTMANAGER_HPP

#include <utility>

class scene;
class AbstractInputEvent;

class IInputManager{
public:
  IInputManager() = default;
  ~IInputManager() = default;
  IInputManager(IInputManager& other) = delete;
  IInputManager(IInputManager&& other) = delete;
  IInputManager& operator=(IInputManager&& other) = delete;

  virtual void handleInput(scene&) = 0;

  virtual void init(){}

  virtual unsigned int getKeyScanCode(unsigned int){return 0;}

  virtual InputKey getPressedKey(){return KEY_NONE;}

  virtual std::pair<std::string, unsigned int>
  handleTextInput(std::string, unsigned int, const AbstractInputEvent&){return {"", 0};}

};


#endif

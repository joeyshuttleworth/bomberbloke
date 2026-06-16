#ifndef IINPUTMANAGER_HPP
#define IINPUTMANAGER_HPP

#include "AbstractInputEvent.hpp"

#include <utility>
#include <string>

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

  virtual InputKey getKeyScanCode(unsigned int i){return (InputKey) i;}

  virtual const std::pair<std::string, int>
  handleTextInput(std::string, unsigned int, const AbstractInputEvent&)
    const
  {return {"", 0};}

  void setNextBindCommand(std::string& com)
  {
    mNextBindCommand = com;
  }

protected:
  std::string mNextBindCommand;
  const int DEADZONE = 9000;
};


#endif

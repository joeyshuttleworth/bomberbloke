#ifndef DUMMYINPUTMANAGER_HPP
#define DUMMYINPUTMANAGER_HPP

#include "IInputManager.hpp"

class scene;

class DummyInputManager : public IInputManager{
public:
  using IInputManager::IInputManager;
  using IInputManager::operator=;

  void handleInput(scene&) override{}
};


#endif

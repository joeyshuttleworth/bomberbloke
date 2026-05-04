#ifndef SDLINPUTMANAGER_HPP
#define SDLINPUTMANAGER_HPP

#include <cstdint>
#include <pair>
#include "IInputManager.hpp"

class scene;

class SDLInputManager : IInputManager{
public:
  using IInputManager::IInputManager;
  using IInputManager::operator=;

  void handleInput(scene&) override;

  void init() override;

  std::pair<std::string, unsigned int>
  handleTextInput(std::string field_text, unsigned int cursor_pos,
                  const AbstactInputEvent& _event)
    override;

protected:
  uint8_t* _kb_state = nullptr;
  bool mBindNextKey = false;
};


#endif

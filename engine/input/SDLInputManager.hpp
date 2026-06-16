#ifndef SDLINPUTMANAGER_HPP
#define SDLINPUTMANAGER_HPP

#include <cstdint>
#include <utility>

#include "IInputManager.hpp"

class scene;

class SDLInputManager : public IInputManager{
public:
  using IInputManager::IInputManager;
  using IInputManager::operator=;

  void handleInput(scene&) override;

  void init() override;

  const std::pair<std::string, int>
  handleTextInput(std::string field_text, unsigned int cursor_pos,
                  const AbstractInputEvent& _event) const
    override;

  ~SDLInputManager() {delete _kb_state;}

protected:
  uint8_t* _kb_state = nullptr;
  bool mBindNextKey = false;
};


#endif

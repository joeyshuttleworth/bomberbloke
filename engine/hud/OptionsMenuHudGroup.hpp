#ifndef OPTIONSMENUHUDGROUP_HPP
#define OPTIONSMENUHUDGROUP_HPP

#include "AbstractHudGroup.hpp"

#include <functional>
#include <memory>

#include "SoundManager.hpp"

class TextHudElement;
extern bool handle_system_command(std::list<std::string>);
extern SoundManager soundManager;

class OptionsMenuHudGroup: public AbstractHudGroup {
public:
  /**
   * Constructor for OptionsMenuHudGroup.
   */
  OptionsMenuHudGroup(std::function<void()> goBackFn);

private:
  // Weak pointer to the text HUD element displaying the current master volume.
  std::weak_ptr<TextHudElement> mMasterVolumeLabel;

  /**
   * Reduces the master volume by 1 unit.
   */
  void reduceMasterVolume();

  /**
   * Increases the master volume by 1 unit.
   */
  void increaseMasterVolume();
};

#endif

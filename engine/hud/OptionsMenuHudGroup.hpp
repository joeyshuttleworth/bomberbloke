#ifndef OPTIONSMENUHUDGROUP_HPP
#define OPTIONSMENUHUDGROUP_HPP

#include "AbstractHudGroup.hpp"

#include <functional>
#include <memory>

class TextHudElement;
class SoundManager;
class TextButton;
class InputField;

extern bool handle_system_command(std::list<std::string>);
extern SoundManager soundManager;

class OptionsMenuHudGroup: public AbstractHudGroup {
public:
  /**
   * Constructor for OptionsMenuHudGroup.
   */
  OptionsMenuHudGroup(scene& r_scene, std::function<void()> goBackFn);

protected:
  // Weak pointer to the text HUD element displaying the current master volume.
  std::weak_ptr<TextHudElement> mMasterVolumeLabel;

  // Weak pointer to the text HUD element displaying the current music volume.
  std::weak_ptr<TextHudElement> mMusicVolumeLabel;
  // Weak pointer to window mode button.
  std::weak_ptr<TextButton> mWindowModeButton;

  // Weak pointer to console field.
  std::weak_ptr<InputField> mConsoleField;

  /**
   * Reduces the master volume by 1 unit.
   */
  void reduceMasterVolume();

  /**
   * Increases the master volume by 1 unit.
   */
  void increaseMasterVolume();

  /**
   * Reduces the music volume by 1 unit.
   */
  void reduceMusicVolume();

  /**
   * Increases the master volume by 1 unit.
   */
  void increaseMusicVolume();

  /**
   * Toggles between fullscreen and windowed.
   */
  void toggleWindowMode();

  /**
   * Handles command given in console input field.
   */
  void handleConsoleInput();

  std::string mFont = "Aileron-Black";
  int mFontSize = 12;
};

#endif

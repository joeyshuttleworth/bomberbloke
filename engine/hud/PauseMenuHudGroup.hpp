#ifndef PAUSEMENUHUDGROUP_HPP
#define PAUSEMENUHUDGROUP_HPP

#include "AbstractHudGroup.hpp"

#include <memory>

class OptionsMenuHudGroup;

class PauseMenuHudGroup: public AbstractHudGroup {
public:
  /**
   * Constructor for PauseMenuHudGroup.
   */
  PauseMenuHudGroup();

private:
  // Weak pointer to the options menu HUD group stored in mHudElements.
  std::weak_ptr<OptionsMenuHudGroup> mOptionsMenu;

  /**
   * Opens the options menu HUD group.
   */
  void openOptionsMenu();

  /**
   * Closes the options menu HUD group.
   */
  void closeOptionsMenu();
};

#endif

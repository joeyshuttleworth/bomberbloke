#ifndef MAINMENUHUDGROUP_HPP
#define MAINMENUHUDGROUP_HPP

#include "AbstractHudGroup.hpp"

#include <memory>

class OptionsMenuHudGroup;
class JoinMenuHudGroup;
class TextHudElement;

class MainMenuHudGroup: public AbstractHudGroup {
public:
  /**
   * Constructor for PauseMenuHudGroup.
   */
  MainMenuHudGroup();

protected:
  // Weak pointer to the options menu HUD group stored in mHudElements.
  std::weak_ptr<OptionsMenuHudGroup> mOptionsMenu;

  // Weak pointer to the options menu HUD group stored in mHudElements.
  std::weak_ptr<JoinMenuHudGroup> mJoinMenu;

  // Weak pointer to the loading text hud element stored in mHudElements.
  std::weak_ptr<TextHudElement> mLoadingText;

  /**
   * Opens the join menu HUD group.
   */
  void showJoinMenu();

  /**
   * Opens the options menu HUD group.
   */
  void showOptionsMenu();

  /**
   * Closes the options and join menu HUD group and shows the main menu.
   */
  void showMainMenu();
};

#endif

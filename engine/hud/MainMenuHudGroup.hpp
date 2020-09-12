#ifndef MAINMENUHUDGROUP_HPP
#define MAINMENUHUDGROUP_HPP

#include "AbstractHudGroup.hpp"

#include <memory>

class OptionsMenuHudGroup;
class TextHudElement;

class MainMenuHudGroup: public AbstractHudGroup {
public:
  /**
   * Constructor for PauseMenuHudGroup.
   */
  MainMenuHudGroup();

  /**
   * Update method to be called on every tick.
   */
  void update() override;

protected:
  // Weak pointer to the options menu HUD group stored in mHudElements.
  std::weak_ptr<OptionsMenuHudGroup> mOptionsMenu;

  // Weak pointer to the loading text hud element stored in mHudElements.
  std::weak_ptr<TextHudElement> mLoadingText;

  // If set to true the server will be joined on next update.
  bool mJoinServer = false;
  
  // Address of server to be joined.
  std::string mServerAddress = "localhost";

  /**
   * Opens the options menu HUD group.
   */
  void showOptionsMenu();

  /**
   * Shows the loading text.
   */
  void showLoadingText();

  /**
   * Closes the options menu HUD group and shows the main menu.
   */
  void showMainMenu();

  /**
   * Joins the server and starts the game.
   */
  void joinServer();
};

#endif

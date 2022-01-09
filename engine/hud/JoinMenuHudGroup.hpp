#ifndef JOINMENUHUDGROUP_HPP
#define JOINMENUHUDGROUP_HPP

#include "AbstractHudGroup.hpp"

#include <functional>
#include <memory>

class TextManager;
class InputField;
class TextHudElement;
class TextButton;
class NetClient;

extern bool handle_system_command(std::list<std::string>);
extern TextManager textManager;

class JoinMenuHudGroup: public AbstractHudGroup {
public:
  /**
   * Constructor for JoinMenuHudGroup.
   */
  JoinMenuHudGroup(std::function<void()> goBackFn);

  /**
   * Joins server with parameters given in input fields.
   */
  void joinServer();

  /**
   * Update method to be called on every tick.
   */
  void update() override;

protected:
  // Weak pointer to input fields.
  std::weak_ptr<InputField> mNicknameField;
  std::weak_ptr<InputField> mAddressField;
  std::weak_ptr<InputField> mRedField;
  std::weak_ptr<InputField> mBlueField;
  std::weak_ptr<InputField> mGreenField;

  // Weak pointer to loading text.
  std::weak_ptr<TextHudElement> mLoadingText;

  // If set to true the server will be joined on next update.
  bool mJoinServer = false;

  /**
   * Shows the loading text.
   */
  void showLoadingText();

  /**
   * Hides the loading text and shows the join menu.
   */
  void showJoinMenu();
};

#endif

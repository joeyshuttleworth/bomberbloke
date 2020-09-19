#include "MainMenuHudGroup.hpp"

#include <cmath>
#include <string>

#include "MainMenuScene.hpp"
#include "OptionsMenuHudGroup.hpp"
#include "JoinMenuHudGroup.hpp"
#include "TextHudElement.hpp"
#include "TextButton.hpp"
#include "TextManager.hpp"

/**
 * Function that closes the game
 */
void quitFn() {
  bool retVal = handle_system_command({"quit"});
}

MainMenuHudGroup::MainMenuHudGroup()
  : AbstractHudGroup(0, 0) {
  // Create title
  std::shared_ptr<Text> titleText = textManager.createText("Aileron-Black", "BOMBERBLOKE");
  titleText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
  titleText->setTextColour({255, 255, 255});
  titleText->setTextScale(4.);
  std::shared_ptr<TextHudElement> titleElement = std::make_shared<TextHudElement>(titleText, 0, -100, 600, 100, ALIGN_CENTER, ALIGN_CENTER);
  titleElement->setIsPostProcessed(false);
  addElement(titleElement);

  // Create start game button text
  std::shared_ptr<Text> startText = textManager.createText("Aileron-Black", "JOIN GAME");
  startText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
  startText->setTextColour({255, 255, 255});
  startText->setTextScale(1.5);
  // Create start game button element
  auto startGameFunction = std::bind(&MainMenuHudGroup::showJoinMenu, this);
  std::shared_ptr<TextButton> startElement = std::make_shared<TextButton>(startText, 0, -20, 200, 30, startGameFunction, ALIGN_CENTER, ALIGN_CENTER);
  startElement->setMouseOverColour({200, 200, 200});
  startElement->setOnClickOffset(-1, 2);
  addElement(startElement);

  // Create options menu button
  std::shared_ptr<Text> optionsText = textManager.createText("Aileron-Black", "OPTIONS");
  optionsText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
  optionsText->setTextColour({255, 255, 255});
  optionsText->setTextScale(1.5);
  // Create options button element
  auto optionsFunction = std::bind(&MainMenuHudGroup::showOptionsMenu, this);
  std::shared_ptr<TextButton> optionsElement = std::make_shared<TextButton>(optionsText, 0, 20, 200, 30, optionsFunction, ALIGN_CENTER, ALIGN_CENTER);
  optionsElement->setMouseOverColour({200, 200, 200});
  optionsElement->setOnClickOffset(-1, 2);
  addElement(optionsElement);

  // Create exit game button
  std::shared_ptr<Text> quitText = textManager.createText("Aileron-Black", "QUIT");
  quitText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
  quitText->setTextColour({255, 255, 255});
  quitText->setTextScale(1.5);
  // Create exit button element
  std::shared_ptr<TextButton> quitElement = std::make_shared<TextButton>(quitText, 0, 60, 200, 30, quitFn, ALIGN_CENTER, ALIGN_CENTER);
  quitElement->setMouseOverColour({200, 200, 200});
  quitElement->setOnClickOffset(-1, 2);
  addElement(quitElement);

  // Create options menu HUD group
  auto returnToMainMenu = std::bind(&MainMenuHudGroup::showMainMenu, this);
  std::shared_ptr<OptionsMenuHudGroup> optionsMenu = std::make_shared<OptionsMenuHudGroup>(returnToMainMenu);
  optionsMenu->setIsVisible(false);
  optionsMenu->mIsInteractive = false;
  addElement(optionsMenu);
  // Store options menu in weak pointer
  mOptionsMenu = optionsMenu;

  // Create join menu HUD group
  std::shared_ptr<JoinMenuHudGroup> joinMenu = std::make_shared<JoinMenuHudGroup>(returnToMainMenu);
  joinMenu->setIsVisible(false);
  joinMenu->mIsInteractive = false;
  addElement(joinMenu);
  // Store join menu in weak pointer
  mJoinMenu = joinMenu;

  // Make group interactive
  mIsInteractive = true;

  // Make entire group avoid post-processing
  setIsPostProcessed(false);
}

void MainMenuHudGroup::showJoinMenu() {
  // Make all HUD elements invisible and non-interactive
  for (auto i = mHudElements.begin(); i != mHudElements.end(); i++) {
    (*i)->setIsVisible(false);
    (*i)->mIsInteractive = false;
  }

  // Set join menu to visible and interactive
  std::shared_ptr<JoinMenuHudGroup> joinMenu = mJoinMenu.lock();
  joinMenu->setIsVisible(true);
  joinMenu->mIsInteractive = true;
}

void MainMenuHudGroup::showOptionsMenu() {
  // Make all HUD elements invisible and non-interactive
  for (auto i = mHudElements.begin(); i != mHudElements.end(); i++) {
    (*i)->setIsVisible(false);
    (*i)->mIsInteractive = false;
  }

  // Set options menu to visible and interactive
  std::shared_ptr<OptionsMenuHudGroup> optionsMenu = mOptionsMenu.lock();
  optionsMenu->setIsVisible(true);
  optionsMenu->mIsInteractive = true;
}

void MainMenuHudGroup::showMainMenu() {
  // Make all HUD elements visible and interactive
  for (auto i = mHudElements.begin(); i != mHudElements.end(); i++) {
    (*i)->setIsVisible(true);
    (*i)->mIsInteractive = true;
  }

  // Set options menu to invisible and non-interactive
  std::shared_ptr<OptionsMenuHudGroup> optionsMenu = mOptionsMenu.lock();
  optionsMenu->setIsVisible(false);
  optionsMenu->mIsInteractive = false;

  // Set join menu to invisible and non-interactive
  std::shared_ptr<JoinMenuHudGroup> joinMenu = mJoinMenu.lock();
  joinMenu->setIsVisible(false);
  joinMenu->mIsInteractive = false;
}

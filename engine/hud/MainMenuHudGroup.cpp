#include "MainMenuHudGroup.hpp"

#include <cmath>
#include <string>

#include "JoinMenuHudGroup.hpp"
#include "MainMenuScene.hpp"
#include "OptionsMenuHudGroup.hpp"
#include "TextButton.hpp"
#include "TextHudElement.hpp"

/**
 * Function that closes the game
 */
void
quitFn()
{
  handle_system_command({ "quit" });
}

MainMenuHudGroup::MainMenuHudGroup(scene& r_scene)
  : AbstractHudGroup(r_scene, 0, 0)
{
  // Create title
  std::shared_ptr<Text> titleText =
    mpGraphicsManager->createText("BOMBERBLOKE", mFont);
  if (titleText) {
    titleText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
    titleText->setTextColour(0xffffffff);
    titleText->setTextScale(4.);
  }

  std::shared_ptr<TextHudElement> titleElement =
    std::make_shared<TextHudElement>(
                                     r_scene, titleText, 0, -100, 600, 100, ALIGN_CENTER, ALIGN_CENTER);
  titleElement->setIsPostProcessed(false);

  addElement(titleElement);

  // Create start game button text
  std::shared_ptr<Text> startText =
    mpGraphicsManager->createText("JOIN GAME", mFont);
  if (startText) {
    startText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
    startText->setTextColour(0xffffffff);
    startText->setTextScale(1.5);
  }
  // Create start game button element
  auto startGameFunction = std::bind(&MainMenuHudGroup::showJoinMenu, this);
  std::shared_ptr<TextButton> startElement = std::make_shared<TextButton>(r_scene,
    startText, 0, -20, 200, 30, startGameFunction, ALIGN_CENTER, ALIGN_CENTER);
  startElement->setMouseOverColour(0xC8C8C8FF);
  startElement->setOnClickOffset(-1, 2);
  addElement(startElement);

  // Create options menu button
  std::shared_ptr<Text> optionsText =
    mpGraphicsManager->createText("OPTIONS", mFont);
  if (optionsText) {
    optionsText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
    optionsText->setTextColour(0xFFFFFFFF);
    optionsText->setTextScale(1.5);
  }
  // Create options button element
  auto optionsFunction = std::bind(&MainMenuHudGroup::showOptionsMenu, this);
  std::shared_ptr<TextButton> optionsElement = std::make_shared<TextButton>(r_scene,
    optionsText, 0, 20, 200, 30, optionsFunction, ALIGN_CENTER, ALIGN_CENTER);
  optionsElement->setMouseOverColour(0xC8C8C8FF);
  optionsElement->setOnClickOffset(-1, 2);
  addElement(optionsElement);

  // Create exit game button
  std::shared_ptr<Text> quitText =
    mpGraphicsManager->createText("QUIT", mFont);
  if (quitText) {
    quitText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
    quitText->setTextColour(0xFFFFFFFF);
    quitText->setTextScale(1.5);
  }
  // Create exit button element
  std::shared_ptr<TextButton> quitElement = std::make_shared<TextButton>(r_scene,
    quitText, 0, 60, 200, 30, quitFn, ALIGN_CENTER, ALIGN_CENTER);
  quitElement->setMouseOverColour(0xC8C8C8FF);
  quitElement->setOnClickOffset(-1, 2);
  addElement(quitElement);

  // Create options menu HUD group
  auto returnToMainMenu = std::bind(&MainMenuHudGroup::showMainMenu, this);
  std::shared_ptr<OptionsMenuHudGroup> optionsMenu =
    std::make_shared<OptionsMenuHudGroup>(r_scene, returnToMainMenu);
  if (optionsMenu) {
    optionsMenu->setIsVisible(false);
    optionsMenu->mIsInteractive = false;
    addElement(optionsMenu);
    // Store options menu in weak pointer
    mOptionsMenu = optionsMenu;
  }

  // Create join menu HUD group
  std::shared_ptr<JoinMenuHudGroup> joinMenu =
    std::make_shared<JoinMenuHudGroup>(r_scene, returnToMainMenu);
  if (joinMenu) {
    joinMenu->setIsVisible(false);
    joinMenu->mIsInteractive = false;
    addElement(joinMenu);
  }
  // Store join menu in weak pointer
  mJoinMenu = joinMenu;

  // Make group interactive
  mIsInteractive = true;

  // Make entire group avoid post-processing
  setIsPostProcessed(false);
}

void
MainMenuHudGroup::showJoinMenu()
{
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

void
MainMenuHudGroup::showOptionsMenu()
{
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

void
MainMenuHudGroup::showMainMenu()
{
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

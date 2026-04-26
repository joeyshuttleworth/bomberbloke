#include "PauseMenuHudGroup.hpp"

#include "MainMenuScene.hpp"
#include "OptionsMenuHudGroup.hpp"
#include "TextButton.hpp"
#include "TextHudElement.hpp"
#include "engine.hpp"

/**
 * Function that resumes the game by simulating an escape key press
 */
void
resumeFn(scene &scn)
{
  scn.handleCommand("unpause");
}

/**
 * Function that disconnects from the game and returns to the main menu
 */
void
disconnectFn()
{
  // Disconnect
  handle_system_command({ "disconnect" });
}

PauseMenuHudGroup::PauseMenuHudGroup(scene &r_scene)
  : AbstractHudGroup(r_scene, 0, 0)
{
  // Create resume button text
  std::shared_ptr<Text> resumeText =
    mrGraphicsManager.createText("RESUME GAME", mFont);
  if(resumeText){
    resumeText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
    resumeText->setTextColour(0xFFFFFFFF);
    resumeText->setTextScale(1.5);
  }
  // Create resume button element
  auto resume_fn = [&](){resumeFn(mrScene);};
  std::shared_ptr<TextButton> resumeElement = std::make_shared<TextButton>(mrScene,
    resumeText, 0, -40, 200, 30, resume_fn, ALIGN_CENTER, ALIGN_CENTER);
  resumeElement->setMouseOverColour(0xC0C0C0FF);
  resumeElement->setOnClickOffset(-1, 2);
  addElement(resumeElement);

  // Create options menu HUD group
  auto closeOptionsFunction =
    std::bind(&PauseMenuHudGroup::closeOptionsMenu, this);
  std::shared_ptr<OptionsMenuHudGroup> optionsMenu =
    std::make_shared<OptionsMenuHudGroup>(mrScene, closeOptionsFunction);
  optionsMenu->setIsVisible(false);
  optionsMenu->mIsInteractive = false;
  addElement(optionsMenu);
  // Store options menu in weak pointer
  mOptionsMenu = optionsMenu;

  // Make group interactive
  mIsInteractive = true;

  // Make entire group avoid post-processing
  setIsPostProcessed(false);

  // Create leave button text
  std::shared_ptr<Text> leaveText =
    mrGraphicsManager.createText("LEAVE GAME", mFont);

  std::shared_ptr<Text> optionsText =
    mrGraphicsManager.createText("OPTIONS", mFont);

  if(!leaveText || !optionsText){
    log_message(ERR, "Failed to create pause menu text");
  }

  if(leaveText){
    leaveText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
    leaveText->setTextColour(0xFFFFFFFF);
    leaveText->setTextScale(1.5);
  }
  // Create leave button element
  std::shared_ptr<TextButton> leaveElement = std::make_shared<TextButton>(mrScene,
    leaveText, 0, 0, 200, 30, disconnectFn, ALIGN_CENTER, ALIGN_CENTER);
  leaveElement->setMouseOverColour(0xC0C0C0FF);
  leaveElement->setOnClickOffset(-1, 2);
  addElement(leaveElement);

  // Create options menu button
  if(optionsText){
    optionsText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
    optionsText->setTextColour(0xFFFFFFFF);
    optionsText->setTextScale(1.5);
  }

  // Create options button element
  auto optionsFunction = std::bind(&PauseMenuHudGroup::openOptionsMenu, this);
  std::shared_ptr<TextButton> optionsElement = std::make_shared<TextButton>(mrScene,
    optionsText, 0, 40, 200, 30, optionsFunction, ALIGN_CENTER, ALIGN_CENTER);
  optionsElement->setMouseOverColour(0xC0C0C0FF);
  optionsElement->setOnClickOffset(-1, 2);
  addElement(optionsElement);

}

void
PauseMenuHudGroup::openOptionsMenu()
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
PauseMenuHudGroup::closeOptionsMenu()
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
}

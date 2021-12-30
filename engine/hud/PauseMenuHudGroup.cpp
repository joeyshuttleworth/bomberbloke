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
resumeFn()
{
  SDL_Event escapeUpEvent;
  escapeUpEvent.type = SDL_KEYUP;
  escapeUpEvent.key.keysym.sym = SDLK_ESCAPE;

  // Give fake key event to scene
  _pScene->onInput(&escapeUpEvent);
}

/**
 * Function that disconnects from the game and returns to the main menu
 */
void
disconnectFn()
{
  // Move to main menu scene
  _pNewScene = std::make_shared<MainMenuScene>(10, 10);

  // Disconnect
  bool retVal = handle_system_command({ "disconnect" });

  if (!retVal)
    // Disconnect failed
    _pNewScene = nullptr;
}

PauseMenuHudGroup::PauseMenuHudGroup()
  : AbstractHudGroup(0, 0)
{
  // Create resume button text
  std::shared_ptr<Text> resumeText =
    textManager.createText("Aileron-Black", "RESUME GAME");
  if(resumeText){
    resumeText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
    resumeText->setTextColour({ 255, 255, 255, 255 });
    resumeText->setTextScale(1.5);
  }
  // Create resume button element
  std::shared_ptr<TextButton> resumeElement = std::make_shared<TextButton>(
    resumeText, 0, -40, 200, 30, resumeFn, ALIGN_CENTER, ALIGN_CENTER);
  resumeElement->setMouseOverColour({ 200, 200, 200, 255 });
  resumeElement->setOnClickOffset(-1, 2);
  addElement(resumeElement);

  // Create leave button text
  std::shared_ptr<Text> leaveText =
    textManager.createText("Aileron-Black", "LEAVE GAME");
  leaveText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
  leaveText->setTextColour({ 255, 255, 255, 255 });
  leaveText->setTextScale(1.5);
  // Create leave button element
  std::shared_ptr<TextButton> leaveElement = std::make_shared<TextButton>(
    leaveText, 0, 0, 200, 30, disconnectFn, ALIGN_CENTER, ALIGN_CENTER);
  leaveElement->setMouseOverColour({ 200, 200, 200, 255 });
  leaveElement->setOnClickOffset(-1, 2);
  addElement(leaveElement);

  // Create options menu button
  std::shared_ptr<Text> optionsText =
    textManager.createText("Aileron-Black", "OPTIONS");
  optionsText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
  optionsText->setTextColour({ 255, 255, 255, 255 });
  optionsText->setTextScale(1.5);
  // Create options button element
  auto optionsFunction = std::bind(&PauseMenuHudGroup::openOptionsMenu, this);
  std::shared_ptr<TextButton> optionsElement = std::make_shared<TextButton>(
    optionsText, 0, 40, 200, 30, optionsFunction, ALIGN_CENTER, ALIGN_CENTER);
  optionsElement->setMouseOverColour({ 200, 200, 200, 255 });
  optionsElement->setOnClickOffset(-1, 2);
  addElement(optionsElement);

  // Create options menu HUD group
  auto closeOptionsFunction =
    std::bind(&PauseMenuHudGroup::closeOptionsMenu, this);
  std::shared_ptr<OptionsMenuHudGroup> optionsMenu =
    std::make_shared<OptionsMenuHudGroup>(closeOptionsFunction);
  optionsMenu->setIsVisible(false);
  optionsMenu->mIsInteractive = false;
  addElement(optionsMenu);
  // Store options menu in weak pointer
  mOptionsMenu = optionsMenu;

  // Make group interactive
  mIsInteractive = true;

  // Make entire group avoid post-processing
  setIsPostProcessed(false);
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

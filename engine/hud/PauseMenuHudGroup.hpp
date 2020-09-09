#ifndef PAUSEMENUHUDGROUP_HPP
#define PAUSEMENUHUDGROUP_HPP

#include "AbstractHudGroup.hpp"

#include "OptionsMenuHudGroup.hpp"

/**
 * Function that resumes the game by simulating an escape key press
 */
void resumeFn() {
  SDL_Event escapeUpEvent;
  escapeUpEvent.type = SDL_KEYUP;
  escapeUpEvent.key.keysym.sym = SDLK_ESCAPE;

  // Give fake key event to scene
  _pScene->onInput(&escapeUpEvent);
}

class PauseMenuHudGroup: public AbstractHudGroup {
public:
  /**
   * Constructor for PauseMenuHudGroup.
   */
  PauseMenuHudGroup() : AbstractHudGroup(0, 0) {
    // Create resume button text
    std::shared_ptr<Text> resumeText = textManager.createText("Aileron-Black", "RESUME GAME");
    resumeText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
    resumeText->setTextColour({255, 255, 255});
    resumeText->setTextScale(1.5);
    // Create resume button element
    std::shared_ptr<TextButton> resumeElement = std::make_shared<TextButton>(resumeText, 0, 20, 200, 30, resumeFn, ALIGN_CENTER, ALIGN_CENTER);
    resumeElement->setMouseOverColour({200, 200, 200});
    resumeElement->setOnClickOffset(-1, 2);
    addElement(resumeElement);

    // Create options menu button
    std::shared_ptr<Text> optionsText = textManager.createText("Aileron-Black", "OPTIONS");
    optionsText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
    optionsText->setTextColour({255, 255, 255});
    optionsText->setTextScale(1.5);
    // Create options button element
    auto optionsFunction = std::bind(&PauseMenuHudGroup::openOptionsMenu, this);
    std::shared_ptr<TextButton> optionsElement = std::make_shared<TextButton>(optionsText, 0, -20, 200, 30, optionsFunction, ALIGN_CENTER, ALIGN_CENTER);
    optionsElement->setMouseOverColour({200, 200, 200});
    optionsElement->setOnClickOffset(-1, 2);
    addElement(optionsElement);

    // Create options menu HUD group
    auto closeOptionsFunction = std::bind(&PauseMenuHudGroup::closeOptionsMenu, this);
    std::shared_ptr<OptionsMenuHudGroup> optionsMenu = std::make_shared<OptionsMenuHudGroup>(closeOptionsFunction);
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

private:
  // Weak pointer to the options menu HUD group stored in mHudElements.
  std::weak_ptr<OptionsMenuHudGroup> mOptionsMenu;

  /**
   * Opens the options menu HUD group
   */
  void openOptionsMenu() {
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

  /**
   * Closes the options menu HUD group
   */
  void closeOptionsMenu() {
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

};

#endif

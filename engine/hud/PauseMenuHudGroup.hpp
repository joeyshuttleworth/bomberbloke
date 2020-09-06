#ifndef PAUSEMENUHUDGROUP_HPP
#define PAUSEMENUHUDGROUP_HPP

#include "AbstractHudGroup.hpp"

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
    std::shared_ptr<Text> pResumeText = textManager.createText("Aileron-Black", "Resume Game");
    pResumeText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
    pResumeText->setTextColour({255, 255, 255});
    pResumeText->setTextScale(1.5);
    // Create resume button element
    std::shared_ptr<TextButton> resumeElement = std::make_shared<TextButton>(pResumeText, 0, 50, 200, 30, resumeFn, ALIGN_CENTER, ALIGN_CENTER);
    resumeElement->setMouseOverColour({200, 200, 200});
    resumeElement->setOnClickOffset(-1, 2);
    mHudElements.push_back(resumeElement);

    // Make group interactive
    mIsInteractive = true;

    // Make entire group avoid post-processing
    setIsPostProcessed(false);
  }

};

#endif

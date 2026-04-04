#include "CountdownHudGroup.hpp"

#include <string>

#include "Sound.hpp"
#include "TextHudElement.hpp"
#include "engine.hpp"

const std::string SOUND_3_NAME = "countdown_3";
const std::string SOUND_2_NAME = "countdown_2";
const std::string SOUND_1_NAME = "countdown_1";
const std::string SOUND_COMMENCE_NAME = "countdown_commence";

CountdownHudGroup::CountdownHudGroup(scene &r_scene, std::function<void()> onFinished,
                                     int maxGlowAmount)
  : AbstractHudGroup(r_scene, 0, 0)
{
  mOnFinished = onFinished;
  mMaxGlowAmount = maxGlowAmount;

  // Create countdown text
  std::shared_ptr<Text> text = mpGraphicsManager->createText("");
  if(text){
    text->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
    text->setTextColour(0xFFFFFFFF);
    text->setTextScale(4.);
  }

  std::shared_ptr<TextHudElement> countdownText =
    std::make_shared<TextHudElement>(r_scene,
      text, 0, 0, 100, 100, ALIGN_CENTER, ALIGN_CENTER);
  addElement(countdownText);
  mCountdownText = countdownText;

  setIsPostProcessed(true);
  setIsVisible(false);

  // Create countdown sound effects
  m3Sound = soundManager.createSound(SOUND_3_NAME);
  m2Sound = soundManager.createSound(SOUND_2_NAME);
  m1Sound = soundManager.createSound(SOUND_1_NAME);
  mCommenceSound = soundManager.createSound(SOUND_COMMENCE_NAME);
}

void
CountdownHudGroup::start(int nSecs)
{
  mTicksLeft = nSecs * TICK_RATE + 1;
  std::shared_ptr<TextHudElement> text_element = mCountdownText.lock();
  setIsVisible(true);

  text_element->setText(std::to_string(nSecs));
  if(text_element->mText)
    text_element->mText->setGlowAmount(mMaxGlowAmount);
}

void
CountdownHudGroup::update()
{
  AbstractHudGroup::update();

  if (mTicksLeft > 0) {
    mTicksLeft--;

    std::shared_ptr<TextHudElement> text = mCountdownText.lock();

    /* TODO make TextHudElement member function for this */
    if(text->mText)
      text->mText->setGlowAmount(mMaxGlowAmount * (mTicksLeft % TICK_RATE) /
                                 TICK_RATE);

    // Update every second
    if (mTicksLeft % TICK_RATE == 0) {
      int secsLeft = mTicksLeft / TICK_RATE;

      if (secsLeft > 0) {
        // Update text
        text->setText(std::to_string(secsLeft));

        // Play sound effect
        if (secsLeft == 3)
          soundManager.playSound(m3Sound);
        else if (secsLeft == 2)
          soundManager.playSound(m2Sound);
        else if (secsLeft == 1)
          soundManager.playSound(m1Sound);
      } else {
        // Countdown is over
        soundManager.playSound(mCommenceSound);
        setIsVisible(false);
        mTicksLeft = 0;
        mOnFinished();
      }
    }
  }
}

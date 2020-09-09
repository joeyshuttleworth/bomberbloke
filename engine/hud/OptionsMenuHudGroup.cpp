#include "OptionsMenuHudGroup.hpp"

#include <algorithm>
#include <string>

OptionsMenuHudGroup::OptionsMenuHudGroup(std::function<void()> goBackFn)
  : AbstractHudGroup(0, 0) {
  // Create title
  std::shared_ptr<Text> titleText = textManager.createText("Aileron-Black", "OPTIONS");
  titleText->setTextAlignment(TEXT_ALIGN_RIGHT, TEXT_ALIGN_CENTER);
  titleText->setTextColour({255, 255, 255});
  titleText->setTextScale(2.);
  std::shared_ptr<TextHudElement> titleElement = std::make_shared<TextHudElement>(titleText, 0, -50, 200, 50, ALIGN_CENTER, ALIGN_CENTER);
  titleElement->setIsPostProcessed(false);
  mHudElements.push_back(titleElement);

  // Create go back button text
  std::shared_ptr<Text> backText = textManager.createText("Aileron-Black", "BACK");
  backText->setTextAlignment(TEXT_ALIGN_LEFT, TEXT_ALIGN_BOTTOM);
  backText->setTextColour({255, 255, 255});
  backText->setTextScale(1.5);
  // Create go back button element
  std::shared_ptr<TextButton> backElement = std::make_shared<TextButton>(backText, 50, 50, 200, -30, goBackFn, ALIGN_LEFT, ALIGN_BOTTOM);
  backElement->setMouseOverColour({200, 200, 200});
  backElement->setOnClickOffset(-1, 2);
  mHudElements.push_back(backElement);

  // Master volume option title
  std::shared_ptr<Text> volumeTitleText = textManager.createText("Aileron-Black", "MASTER VOLUME:");
  volumeTitleText->setTextAlignment(TEXT_ALIGN_RIGHT, TEXT_ALIGN_CENTER);
  volumeTitleText->setTextColour({255, 255, 255});
  volumeTitleText->setTextScale(1.);
  std::shared_ptr<TextHudElement> volumeTitleElement = std::make_shared<TextHudElement>(volumeTitleText, -105, 0, 200, 50, ALIGN_CENTER, ALIGN_CENTER);
  volumeTitleElement->setIsPostProcessed(false);
  mHudElements.push_back(volumeTitleElement);

  // Create reduce volume text
  std::shared_ptr<Text> reduceVolumeText = textManager.createText("Aileron-Black", "–");
  reduceVolumeText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
  reduceVolumeText->setTextColour({255, 255, 255});
  reduceVolumeText->setTextScale(1.);
  // Create reduce volume element
  auto reduceVolumeFn = std::bind(&OptionsMenuHudGroup::reduceMasterVolume, this);
  std::shared_ptr<TextButton> reduceVolumeElement = std::make_shared<TextButton>(reduceVolumeText, 15, 0, 20, 20, reduceVolumeFn, ALIGN_CENTER, ALIGN_CENTER);
  reduceVolumeElement->setMouseOverColour({200, 200, 200});
  reduceVolumeElement->setOnClickOffset(-1, 2);
  mHudElements.push_back(reduceVolumeElement);

  // Master volume option label
  int currentVolumeLabel = soundManager.getMasterVolume() * 8 / 128;
  std::shared_ptr<Text> volumeLabelText = textManager.createText("Aileron-Black", std::to_string(currentVolumeLabel));
  volumeLabelText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
  volumeLabelText->setTextColour({255, 255, 255});
  volumeLabelText->setTextScale(1.);
  std::shared_ptr<TextHudElement> volumeLabelElement = std::make_shared<TextHudElement>(volumeLabelText, 40, 0, 20, 20, ALIGN_CENTER, ALIGN_CENTER);
  volumeLabelElement->setIsPostProcessed(false);
  mHudElements.push_back(volumeLabelElement);
  mMasterVolumeLabel = volumeLabelElement;

  // Create increase volume text
  std::shared_ptr<Text> increaseVolumeText = textManager.createText("Aileron-Black", "+");
  increaseVolumeText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
  increaseVolumeText->setTextColour({255, 255, 255});
  increaseVolumeText->setTextScale(1.);
  // Create go back button element
  auto increaseVolumeFn = std::bind(&OptionsMenuHudGroup::increaseMasterVolume, this);
  std::shared_ptr<TextButton> increaseVolumeElement = std::make_shared<TextButton>(increaseVolumeText, 65, 0, 20, 20, increaseVolumeFn, ALIGN_CENTER, ALIGN_CENTER);
  increaseVolumeElement->setMouseOverColour({200, 200, 200});
  increaseVolumeElement->setOnClickOffset(-1, 2);
  mHudElements.push_back(increaseVolumeElement);

  // Make group interactive
  mIsInteractive = true;

  // Make entire group avoid post-processing
  setIsPostProcessed(false);
}

void OptionsMenuHudGroup::reduceMasterVolume() {
  // Decrease volume by 128 / 8
  int currentVolume = soundManager.getMasterVolume();
  int newVolumeLabel = currentVolume * 8 / 128 - 1;
  if (newVolumeLabel < 0) {
    newVolumeLabel = 0;
  }
  soundManager.setMasterVolume(newVolumeLabel * 128 / 8);

  // Set new volume label
  std::shared_ptr<TextHudElement> label = mMasterVolumeLabel.lock();
  label->setText(std::to_string(newVolumeLabel));
}

void OptionsMenuHudGroup::increaseMasterVolume() {
  // Increase volume by 128 / 8
  int currentVolume = soundManager.getMasterVolume();
  int newVolumeLabel = currentVolume * 8 / 128 + 1;
  if (newVolumeLabel > 8) {
    newVolumeLabel = 8;
  }
  soundManager.setMasterVolume(newVolumeLabel * 128 / 8);

  // Set new volume label
  std::shared_ptr<TextHudElement> label = mMasterVolumeLabel.lock();
  label->setText(std::to_string(newVolumeLabel));
}

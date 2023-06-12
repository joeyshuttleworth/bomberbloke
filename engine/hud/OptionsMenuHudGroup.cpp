#include "OptionsMenuHudGroup.hpp"

#include <algorithm>
#include <string>

#include "InputField.hpp"
#include "Sound.hpp"
#include "TextButton.hpp"
#include "TextHudElement.hpp"
#include "engine.hpp"

OptionsMenuHudGroup::OptionsMenuHudGroup(std::function<void()> goBackFn)
  : AbstractHudGroup(0, 0)
{
  // Create title
  std::shared_ptr<Text> titleText =
    textManager.createText("OPTIONS");
  if (titleText) {
    titleText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
    titleText->setTextColour({ 255, 255, 255, 255 });
    titleText->setTextScale(2.);
  }

  std::shared_ptr<TextHudElement> titleElement =
    std::make_shared<TextHudElement>(
      titleText, 0, -100, 200, 50, ALIGN_CENTER, ALIGN_CENTER);
  titleElement->setIsPostProcessed(false);
  addElement(titleElement);

  // Create go back button text
  std::shared_ptr<Text> backText =
    textManager.createText("BACK");
  if (backText) {
    backText->setTextAlignment(TEXT_ALIGN_LEFT, TEXT_ALIGN_BOTTOM);
    backText->setTextColour({ 255, 255, 255, 255 });
    backText->setTextScale(1.5);
  }

  // Create go back button element
  std::shared_ptr<TextButton> backElement = std::make_shared<TextButton>(
    backText, 50, -50, 200, 30, goBackFn, ALIGN_LEFT, ALIGN_BOTTOM);
  if (backElement) {
    backElement->setMouseOverColour({ 200, 200, 200, 255 });
    backElement->setOnClickOffset(-1, 2);
    addElement(backElement);
  }

  /* Master Volume */
  // Master volume option title
  std::shared_ptr<Text> volumeTitleText =
    textManager.createText("MASTER VOLUME:");
  if (volumeTitleText) {
    volumeTitleText->setTextAlignment(TEXT_ALIGN_RIGHT, TEXT_ALIGN_CENTER);
    volumeTitleText->setTextColour({ 255, 255, 255, 255 });
    volumeTitleText->setTextScale(1.);
  }
  std::shared_ptr<TextHudElement> volumeTitleElement =
    std::make_shared<TextHudElement>(
      volumeTitleText, -105, 0, 200, 50, ALIGN_CENTER, ALIGN_CENTER);
  volumeTitleElement->setIsPostProcessed(false);
  addElement(volumeTitleElement);

  // Create reduce volume text
  std::shared_ptr<Text> reduceVolumeText =
    textManager.createText("-");
  if (reduceVolumeText) {
    reduceVolumeText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
    reduceVolumeText->setTextColour({ 255, 255, 255, 255 });
    reduceVolumeText->setTextScale(1.);
  }
  // Create reduce volume element
  auto reduceVolumeFn =
    std::bind(&OptionsMenuHudGroup::reduceMasterVolume, this);
  std::shared_ptr<TextButton> reduceVolumeElement =
    std::make_shared<TextButton>(reduceVolumeText,
                                 15,
                                 0,
                                 20,
                                 20,
                                 reduceVolumeFn,
                                 ALIGN_CENTER,
                                 ALIGN_CENTER);
  reduceVolumeElement->setMouseOverColour({ 200, 200, 200, 255 });
  reduceVolumeElement->setOnClickOffset(-1, 2);
  addElement(reduceVolumeElement);

  // Master volume option label
  int currentVolumeLabel = soundManager.getVolume(SOUND_MASTER) * 8 / 128;
  std::shared_ptr<Text> volumeLabelText =
    textManager.createText(std::to_string(currentVolumeLabel));
  if (volumeLabelText) {
    volumeLabelText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
    volumeLabelText->setTextColour({ 255, 255, 255, 255 });
    volumeLabelText->setTextScale(1.);
  }
  std::shared_ptr<TextHudElement> volumeLabelElement =
    std::make_shared<TextHudElement>(
      volumeLabelText, 40, 0, 20, 20, ALIGN_CENTER, ALIGN_CENTER);
  volumeLabelElement->setIsPostProcessed(false);
  addElement(volumeLabelElement);
  mMasterVolumeLabel = volumeLabelElement;

  // Create increase volume text
  std::shared_ptr<Text> increaseVolumeText =
    textManager.createText("+");
  if (increaseVolumeText) {
    increaseVolumeText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
    increaseVolumeText->setTextColour({ 255, 255, 255, 255 });
    increaseVolumeText->setTextScale(1.);
  }
  // Create go back button element
  auto increaseVolumeFn =
    std::bind(&OptionsMenuHudGroup::increaseMasterVolume, this);
  std::shared_ptr<TextButton> increaseVolumeElement =
    std::make_shared<TextButton>(increaseVolumeText,
                                 65,
                                 0,
                                 20,
                                 20,
                                 increaseVolumeFn,
                                 ALIGN_CENTER,
                                 ALIGN_CENTER);
  increaseVolumeElement->setMouseOverColour({ 200, 200, 200, 255 });
  increaseVolumeElement->setOnClickOffset(-1, 2);
  addElement(increaseVolumeElement);

  /* Music Volume */
  // Music volume option title
  std::shared_ptr<Text> musicTitleText =
    textManager.createText("MUSIC VOLUME:");
  if (musicTitleText) {
    musicTitleText->setTextAlignment(TEXT_ALIGN_RIGHT, TEXT_ALIGN_CENTER);
    musicTitleText->setTextColour({ 255, 255, 255, 255 });
    musicTitleText->setTextScale(1.);
  }
  std::shared_ptr<TextHudElement> musicTitleElement =
    std::make_shared<TextHudElement>(
      musicTitleText, -105, 50, 200, 50, ALIGN_CENTER, ALIGN_CENTER);
  musicTitleElement->setIsPostProcessed(false);
  addElement(musicTitleElement);

  // Create reduce volume text
  std::shared_ptr<Text> reduceMusicText =
    textManager.createText("-");
  if (reduceMusicText) {
    reduceMusicText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
    reduceMusicText->setTextColour({ 255, 255, 255, 255 });
    reduceMusicText->setTextScale(1.);
  }

  // Create reduce volume element
  auto reduceMusicFn = std::bind(&OptionsMenuHudGroup::reduceMusicVolume, this);
  std::shared_ptr<TextButton> reduceMusicElement = std::make_shared<TextButton>(
    reduceMusicText, 15, 50, 20, 20, reduceMusicFn, ALIGN_CENTER, ALIGN_CENTER);
  if (reduceMusicElement) {
    reduceMusicElement->setMouseOverColour({ 200, 200, 200, 255 });
    reduceMusicElement->setOnClickOffset(-1, 2);
    addElement(reduceMusicElement);
  }

  // Master volume option label
  int currentMusicLabel = soundManager.getVolume(SOUND_MUSIC) * 8 / 128;
  std::shared_ptr<Text> musicLabelText =
    textManager.createText(std::to_string(currentMusicLabel));
  if (musicLabelText) {
    musicLabelText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
    musicLabelText->setTextColour({ 255, 255, 255, 255 });
    musicLabelText->setTextScale(1.);
    std::shared_ptr<TextHudElement> musicLabelElement =
      std::make_shared<TextHudElement>(
        musicLabelText, 40, 50, 20, 20, ALIGN_CENTER, ALIGN_CENTER);
    musicLabelElement->setIsPostProcessed(false);
    addElement(musicLabelElement);
    mMusicVolumeLabel = musicLabelElement;
  }

  // Create increase volume text
  std::shared_ptr<Text> increaseMusicText =
    textManager.createText("+");
  if (increaseMusicText) {
    increaseMusicText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
    increaseMusicText->setTextColour({ 255, 255, 255, 255 });
    increaseMusicText->setTextScale(1.);
  }
  // Create go back button element
  auto increaseMusicFn =
    std::bind(&OptionsMenuHudGroup::increaseMusicVolume, this);
  std::shared_ptr<TextButton> increaseMusicElement =
    std::make_shared<TextButton>(increaseMusicText,
                                 65,
                                 50,
                                 20,
                                 20,
                                 increaseMusicFn,
                                 ALIGN_CENTER,
                                 ALIGN_CENTER);
  increaseMusicElement->setMouseOverColour({ 200, 200, 200, 255 });
  increaseMusicElement->setOnClickOffset(-1, 2);
  addElement(increaseMusicElement);

  /* Window mode setting */
  // Create window mode label
  std::shared_ptr<Text> windowLabelText =
    textManager.createText("WINDOW MODE:");
  if (windowLabelText) {
    windowLabelText->setTextAlignment(TEXT_ALIGN_RIGHT, TEXT_ALIGN_CENTER);
    windowLabelText->setTextColour({ 255, 255, 255, 255 });
    windowLabelText->setTextScale(1.);
  }
  std::shared_ptr<TextHudElement> windowLabelElement =
    std::make_shared<TextHudElement>(
      windowLabelText, -105, 100, 200, 50, ALIGN_CENTER, ALIGN_CENTER);
  windowLabelElement->setIsPostProcessed(false);
  addElement(windowLabelElement);

  // Create window mode button
  std::string buttonString = "";
  if (SDL_GetWindowFlags(_window) & SDL_WINDOW_FULLSCREEN_DESKTOP) {
    buttonString = "FULLSCREEN";
  } else {
    buttonString = "WINDOWED";
  }
  std::shared_ptr<Text> windowModeText =
    textManager.createText(buttonString);
  if (windowModeText) {
    windowModeText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
    windowModeText->setTextColour({ 255, 255, 255, 255 });
    windowModeText->setTextScale(1.);
  }
  auto windowModeFn = std::bind(&OptionsMenuHudGroup::toggleWindowMode, this);
  std::shared_ptr<TextButton> windowModeButton = std::make_shared<TextButton>(
    windowModeText, 55, 100, 100, 50, windowModeFn, ALIGN_CENTER, ALIGN_CENTER);
  windowModeButton->setMouseOverColour({ 200, 200, 200, 255 });
  windowModeButton->setOnClickOffset(-1, 2);
  addElement(windowModeButton);
  mWindowModeButton = windowModeButton;

  /* debug console */
  // Create debug console text
  std::shared_ptr<Text> consoleText =
    textManager.createText("CONSOLE");
  if (consoleText) {
    consoleText->setTextAlignment(TEXT_ALIGN_LEFT, TEXT_ALIGN_CENTER);
    consoleText->setTextColour({ 255, 255, 255, 128 });
    consoleText->setBackgroundColour({ 63, 63, 63, 191 });
    consoleText->setTextOffset(6, 0);
    consoleText->setTextScale(1.);
  }
  // Create console input field
  std::shared_ptr<InputField> consoleField = std::make_shared<InputField>(
    consoleText, -25, 150, 400, 30, ALIGN_CENTER, ALIGN_CENTER);
  consoleField->setInputColour({ 255, 255, 255, 255 });
  addElement(consoleField);
  mConsoleField = consoleField;

  // Create debug console enter button
  std::shared_ptr<Text> consoleEnterText =
    textManager.createText("->");
  if (consoleEnterText) {
    consoleEnterText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
    consoleEnterText->setTextColour({ 255, 255, 255, 255 });
    consoleEnterText->setTextScale(1.);
  }
  // Create go back button element
  auto consoleEnterFn =
    std::bind(&OptionsMenuHudGroup::handleConsoleInput, this);
  std::shared_ptr<TextButton> consoleEnterButton =
    std::make_shared<TextButton>(consoleEnterText,
                                 190,
                                 150,
                                 20,
                                 50,
                                 consoleEnterFn,
                                 ALIGN_CENTER,
                                 ALIGN_CENTER);
  consoleEnterButton->setMouseOverColour({ 200, 200, 200, 255 });
  consoleEnterButton->setOnClickOffset(-1, 2);
  addElement(consoleEnterButton);

  // Make group interactive
  mIsInteractive = true;

  // Make entire group avoid post-processing
  setIsPostProcessed(false);
}

void
OptionsMenuHudGroup::reduceMasterVolume()
{
  // Decrease volume by 128 / 8
  int currentVolume = soundManager.getVolume(SOUND_MASTER);
  int newVolumeLabel = currentVolume * 8 / 128 - 1;
  if (newVolumeLabel < 0) {
    newVolumeLabel = 0;
  }
  soundManager.setVolume(newVolumeLabel * 128 / 8, SOUND_MASTER);

  // Set new volume label
  std::shared_ptr<TextHudElement> label = mMasterVolumeLabel.lock();
  label->setText(std::to_string(newVolumeLabel));
}

void
OptionsMenuHudGroup::increaseMasterVolume()
{
  // Increase volume by 128 / 8
  int currentVolume = soundManager.getVolume(SOUND_MASTER);
  int newVolumeLabel = currentVolume * 8 / 128 + 1;
  if (newVolumeLabel > 8) {
    newVolumeLabel = 8;
  }
  soundManager.setVolume(newVolumeLabel * 128 / 8, SOUND_MASTER);

  // Set new volume label
  std::shared_ptr<TextHudElement> label = mMasterVolumeLabel.lock();
  label->setText(std::to_string(newVolumeLabel));
}

void
OptionsMenuHudGroup::reduceMusicVolume()
{
  // Decrease volume by 128 / 8
  int currentVolume = soundManager.getVolume(SOUND_MUSIC);
  int newVolumeLabel = currentVolume * 8 / 128 - 1;
  if (newVolumeLabel < 0) {
    newVolumeLabel = 0;
  }
  soundManager.setVolume(newVolumeLabel * 128 / 8, SOUND_MUSIC);

  // Set new volume label
  std::shared_ptr<TextHudElement> label = mMusicVolumeLabel.lock();
  label->setText(std::to_string(newVolumeLabel));
}

void
OptionsMenuHudGroup::increaseMusicVolume()
{
  // Increase volume by 128 / 8
  int currentVolume = soundManager.getVolume(SOUND_MUSIC);
  int newVolumeLabel = currentVolume * 8 / 128 + 1;
  if (newVolumeLabel > 8) {
    newVolumeLabel = 8;
  }
  soundManager.setVolume(newVolumeLabel * 128 / 8, SOUND_MUSIC);

  // Set new volume label
  std::shared_ptr<TextHudElement> label = mMusicVolumeLabel.lock();
  label->setText(std::to_string(newVolumeLabel));
}

void
OptionsMenuHudGroup::toggleWindowMode()
{
  std::shared_ptr<TextButton> windowModeButton = mWindowModeButton.lock();
  if (SDL_GetWindowFlags(_window) & SDL_WINDOW_FULLSCREEN_DESKTOP) {
    // Window is full screen, make it windowed
    SDL_SetWindowFullscreen(_window, 0);
    windowModeButton->setText("WINDOWED");
  } else {
    // Window is not full screen, make it full screen
    SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    int w, h;
    SDL_GetWindowSize(_window, &w, &h);
    handle_system_command({ "resize", std::to_string(w), std::to_string(h) });
    windowModeButton->setText("FULLSCREEN");
  }
}

void
OptionsMenuHudGroup::handleConsoleInput()
{
  std::shared_ptr<InputField> consoleField = mConsoleField.lock();
  std::string command = consoleField->mText->getText();
  handle_system_command(split_to_tokens(command));
}

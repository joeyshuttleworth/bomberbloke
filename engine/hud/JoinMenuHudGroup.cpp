#include "JoinMenuHudGroup.hpp"

#include <string>

#include "BomberBlokeScene.hpp"
#include "InputField.hpp"
#include "NetClient.hpp"
#include "Text.hpp"
#include "TextButton.hpp"
#include "TextManager.hpp"
#include "engine.hpp"
#include <sstream>

JoinMenuHudGroup::JoinMenuHudGroup(std::function<void()> goBackFn)
  : AbstractHudGroup(0, 0), mGoBackFn(goBackFn)
{
  // Create nickname text
  std::shared_ptr<Text> nicknameFieldText =
    textManager.createText("NICKNAME");
  if (nicknameFieldText) {
    nicknameFieldText->setTextAlignment(TEXT_ALIGN_LEFT, TEXT_ALIGN_CENTER);
    nicknameFieldText->setTextColour({ 255, 255, 255, 128 });
    nicknameFieldText->setBackgroundColour({ 63, 63, 63, 191 });
    nicknameFieldText->setTextOffset(10, 0);
    nicknameFieldText->setTextScale(1.5);
  }
  // Create nickname input field
  std::shared_ptr<InputField> nicknameField = std::make_shared<InputField>(
    nicknameFieldText, 0, -60, 300, 50, ALIGN_CENTER, ALIGN_CENTER);
  nicknameField->setInputColour({ 255, 255, 255, 255 });
  nicknameField->setInputText(_nickname);
  addElement(nicknameField);
  mNicknameField = nicknameField;

  // Create red text
  std::shared_ptr<Text> redFieldText =
    textManager.createText("R");
  if (redFieldText) {
    redFieldText->setTextAlignment(TEXT_ALIGN_LEFT, TEXT_ALIGN_CENTER);
    redFieldText->setTextColour({ 255, 255, 255, 128 });
    redFieldText->setBackgroundColour({ 63, 63, 63, 191 });
    redFieldText->setTextOffset(10, 0);
    redFieldText->setTextScale(1.5);
  }
  // Create red input field
  std::shared_ptr<InputField> redField = std::make_shared<InputField>(
    redFieldText, -(66+12)/2*3, 0, 66, 50, ALIGN_CENTER, ALIGN_CENTER);
  redField->setInputColour({ 255, 255, 255, 255 });
  addElement(redField);
  mRedField = redField;

  // Create green text
  std::shared_ptr<Text> greenFieldText =
    textManager.createText("G");
  if (greenFieldText) {
    greenFieldText->setTextAlignment(TEXT_ALIGN_LEFT, TEXT_ALIGN_CENTER);
    greenFieldText->setTextColour({ 255, 255, 255, 128 });
    greenFieldText->setBackgroundColour({ 63, 63, 63, 191 });
    greenFieldText->setTextOffset(10, 0);
    greenFieldText->setTextScale(1.5);
  }
  // Create green input field
  std::shared_ptr<InputField> greenField = std::make_shared<InputField>(
    greenFieldText, -(66+12)/2, 0, 66, 50, ALIGN_CENTER, ALIGN_CENTER);
  greenField->setInputColour({ 255, 255, 255, 255 });
  addElement(greenField);
  mGreenField = greenField;

  // Create blue text
  std::shared_ptr<Text> blueFieldText =
    textManager.createText("B");
  if (blueFieldText) {
    blueFieldText->setTextAlignment(TEXT_ALIGN_LEFT, TEXT_ALIGN_CENTER);
    blueFieldText->setTextColour({ 255, 255, 255, 128 });
    blueFieldText->setBackgroundColour({ 63, 63, 63, 191 });
    blueFieldText->setTextOffset(10, 0);
    blueFieldText->setTextScale(1.5);
  }
  // Create blue input field
  std::shared_ptr<InputField> blueField = std::make_shared<InputField>(
    blueFieldText, (66+12)/2, 0, 66, 50, ALIGN_CENTER, ALIGN_CENTER);
  blueField->setInputColour({ 255, 255, 255, 255 });
  addElement(blueField);
  mBlueField = blueField;

  // Create colour button text
  std::shared_ptr<Text> colourText = textManager.createText("");
  if (colourText) {
    colourText->setBackgroundColour({ 255, 255, 255, 255 });
  }
  // Create colour button element
  auto randomColourFunction = std::bind(&JoinMenuHudGroup::pickRandomColour, this);
  std::shared_ptr<TextButton> colourButton = std::make_shared<TextButton>(
    colourText, (66+12)/2*3, 0, 66, 50, randomColourFunction, ALIGN_CENTER, ALIGN_CENTER);
  addElement(colourButton);
  mColourButton = colourButton;

  // Create detect click button
  // TODO: find a better solution for detecting clicks
  auto updateColourFunction = std::bind(&JoinMenuHudGroup::updateColourButton, this);
  std::shared_ptr<ClickableHudElement> detectClickButton = std::make_shared<ClickableHudElement>(
    0, 0, 500, 240, updateColourFunction, ALIGN_CENTER, ALIGN_CENTER);
  addElement(detectClickButton);

  // Create address text
  std::shared_ptr<Text> addressFieldText =
    textManager.createText("ADDRESS");
  if (addressFieldText) {
    addressFieldText->setTextAlignment(TEXT_ALIGN_LEFT, TEXT_ALIGN_CENTER);
    addressFieldText->setTextColour({ 255, 255, 255, 128 });
    addressFieldText->setBackgroundColour({ 63, 63, 63, 191 });
    addressFieldText->setTextOffset(10, 0);
    addressFieldText->setTextScale(1.5);
  }
  // Create address input field
  std::shared_ptr<InputField> addressField = std::make_shared<InputField>(
    addressFieldText, 0, 60, 300, 50, ALIGN_CENTER, ALIGN_CENTER);
  addressField->setInputColour({ 255, 255, 255, 255 });
  addressField->setInputText(_net_client->mServerAddress);
  addElement(addressField);
  mAddressField = addressField;

  // Create join text
  std::shared_ptr<Text> joinText =
    textManager.createText("JOIN");
  if (joinText) {
    joinText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
    joinText->setTextColour({ 255, 255, 255, 255 });
    joinText->setTextScale(1.5);
  }
  // Create join button
  auto joinFn = std::bind(&JoinMenuHudGroup::joinServer, this);
  std::shared_ptr<TextButton> joinElement = std::make_shared<TextButton>(
    joinText, 0, 120, 100, 50, joinFn, ALIGN_CENTER, ALIGN_CENTER);
  joinElement->setMouseOverColour({ 200, 200, 200, 255 });
  joinElement->setOnClickOffset(-1, 2);
  addElement(joinElement);

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
  backElement->setMouseOverColour({ 200, 200, 200, 255 });
  backElement->setOnClickOffset(-1, 2);
  addElement(backElement);

  // Create loading text
  std::shared_ptr<Text> loadingText =
    textManager.createText("LOADING...");
  if (loadingText) {
    loadingText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
    loadingText->setTextColour({ 255, 255, 255, 255 });
    loadingText->setTextScale(3.);
  }

  // Create loading text element
  std::shared_ptr<TextHudElement> loadingElement =
    std::make_shared<TextHudElement>(
      loadingText, 0, 0, 400, 80, ALIGN_CENTER, ALIGN_CENTER);
  loadingElement->setIsPostProcessed(false);
  loadingElement->setIsVisible(false);
  mLoadingText = loadingElement;
  addElement(loadingElement);

  pickRandomColour();
}

void
JoinMenuHudGroup::onInput(SDL_Event* event){
  AbstractHudGroup::onInput(event);

  if (event->type == SDL_KEYDOWN){
    if(event->key.keysym.sym == SDLK_ESCAPE){
      mGoBackFn();
      return;
    } else if (event->key.keysym.sym == SDLK_TAB){
      // tab to next hud element
    }
  }
}


void
JoinMenuHudGroup::update()
{
  AbstractHudGroup::update();

  if (mJoinServer) {
    // Get nickname in text field
    std::shared_ptr<InputField> nicknameField = mNicknameField.lock();
    std::string nickname = nicknameField->mText->getText();

    // Get red value in text field
    std::shared_ptr<InputField> redField = mRedField.lock();
    std::string redString = redField->mText->getText();

    // Get green in text field
    std::shared_ptr<InputField> greenField = mGreenField.lock();
    std::string greenString = greenField->mText->getText();

    // Get blue value in text field
    std::shared_ptr<InputField> blueField = mBlueField.lock();
    std::string blueString = blueField->mText->getText();

    // Get address in text field
    std::shared_ptr<InputField> addressField = mAddressField.lock();
    std::string address = addressField->mText->getText();
    if(address == "ADDRESS")
      address = "localhost";

    // Set nickname
    handle_system_command({ "nickname", nickname });

    // Convert colour to correct, proper format : 0xrrggbbaa
    Uint8 redValue, greenValue, blueValue;
    try {
      redValue = (Uint8) std::stoi(redString);
      greenValue = (Uint8) std::stoi(greenString);
      blueValue = (Uint8) std::stoi(blueString);
    } catch (std::invalid_argument &e) {
        return;
    }

    // Set colour
    Uint32 colour = (redValue << 24) + (greenValue << 16) + (blueValue << 8);
    colour |= 0xFF;
    std::stringstream colour_stream;
    colour_stream << "colour " << std::hex << colour;
    // handle_system_command({ "colour", colour_stream.str() });

    std::vector<std::string> commands = {colour_stream.str()};

    if(_net_client->joinBlokeServer(address, _nickname, commands)){
    // If successful move to bomberbloke scene
    _pNewScene = std::make_shared<BomberBlokeScene>(10, 10);
    } else{
      // If failed go back to main menu
      showJoinMenu();
      mJoinServer = false;
    }
  }
}

void
JoinMenuHudGroup::pickRandomColour()
{
  int redValue = std::rand() % 256;
  int greenValue = std::rand() % 256;
  int blueValue = std::rand() % 256;

  std::shared_ptr<InputField> redField = mRedField.lock();
  redField->setInputText(std::to_string(redValue));

  std::shared_ptr<InputField> greenField = mGreenField.lock();
  greenField->setInputText(std::to_string(greenValue));

  std::shared_ptr<InputField> blueField = mBlueField.lock();
  blueField->setInputText(std::to_string(blueValue));

  updateColourButton();
}

void
JoinMenuHudGroup::updateColourButton()
{
  std::shared_ptr<InputField> redField = mRedField.lock();
  std::shared_ptr<InputField> greenField = mGreenField.lock();
  std::shared_ptr<InputField> blueField = mBlueField.lock();

  int redValue, greenValue, blueValue;

  // TODO : find a better solution
  try {
    redValue = std::stoi(redField->mText->getText());
    greenValue = std::stoi(greenField->mText->getText());
    blueValue = std::stoi(blueField->mText->getText());
  } catch (std::invalid_argument &e) {
    return;
  }

  if (redValue < 256 && redValue >= 0
      && greenValue < 256 && greenValue >= 0
      && blueValue < 256 && blueValue >= 0) {
    std::shared_ptr<TextButton> colourButton = mColourButton.lock();
    SDL_Color colour({(Uint8) redValue, (Uint8) greenValue, (Uint8) blueValue, 255});
    colourButton->mText->setBackgroundColour(colour);
  }
}

void
JoinMenuHudGroup::joinServer()
{
  showLoadingText();
  mJoinServer = true;
}

void
JoinMenuHudGroup::showLoadingText()
{
  // Make all HUD elements invisible and non-interactive
  for (auto i = mHudElements.begin(); i != mHudElements.end(); i++) {
    (*i)->setIsVisible(false);
    (*i)->mIsInteractive = false;
  }

  // Set loading text visible
  std::shared_ptr<TextHudElement> loadingText = mLoadingText.lock();
  loadingText->setIsVisible(true);
}

void
JoinMenuHudGroup::showJoinMenu()
{
  // Make all HUD elements visible and interactive
  for (auto i = mHudElements.begin(); i != mHudElements.end(); i++) {
    (*i)->setIsVisible(true);
    (*i)->mIsInteractive = true;
  }

  // Set loading text visible
  std::shared_ptr<TextHudElement> loadingText = mLoadingText.lock();
  loadingText->setIsVisible(false);
}

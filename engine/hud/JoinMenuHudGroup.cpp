#include "JoinMenuHudGroup.hpp"

#include <string>

#include "BomberBlokeScene.hpp"
#include "InputField.hpp"
#include "TextButton.hpp"
#include "engine.hpp"
#include "Text.hpp"
#include "NetClient.hpp"
#include "TextManager.hpp"

JoinMenuHudGroup::JoinMenuHudGroup(std::function<void()> goBackFn)
  : AbstractHudGroup(0, 0) {
  // Create nickname text
  std::shared_ptr<Text> nicknameFieldText = textManager.createText("Aileron-Black", "NICKNAME");
  nicknameFieldText->setTextAlignment(TEXT_ALIGN_LEFT, TEXT_ALIGN_CENTER);
  nicknameFieldText->setTextColour({255, 255, 255, 128});
  nicknameFieldText->setBackgroundColour({63, 63, 63, 191});
  nicknameFieldText->setTextOffset(10, 0);
  nicknameFieldText->setTextScale(1.5);
  // Create nickname input field
  std::shared_ptr<InputField> nicknameField = std::make_shared<InputField>(nicknameFieldText, 0, -60, 300, 50, ALIGN_CENTER, ALIGN_CENTER);
  nicknameField->setInputColour({255, 255, 255});
  nicknameField->setInputText(_nickname);
  addElement(nicknameField);
  mNicknameField = nicknameField;

  // Create address text
  std::shared_ptr<Text> addressFieldText = textManager.createText("Aileron-Black", "ADDRESS");
  addressFieldText->setTextAlignment(TEXT_ALIGN_LEFT, TEXT_ALIGN_CENTER);
  addressFieldText->setTextColour({255, 255, 255, 128});
  addressFieldText->setBackgroundColour({63, 63, 63, 191});
  addressFieldText->setTextOffset(10, 0);
  addressFieldText->setTextScale(1.5);
  // Create address input field
  std::shared_ptr<InputField> addressField = std::make_shared<InputField>(addressFieldText, 0, 0, 300, 50, ALIGN_CENTER, ALIGN_CENTER);
  addressField->setInputColour({255, 255, 255});
  addressField->setInputText(_net_client.mServerAddress);
  addElement(addressField);
  mAddressField = addressField;

  // Create join text
  std::shared_ptr<Text> joinText = textManager.createText("Aileron-Black", "JOIN");
  joinText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
  joinText->setTextColour({255, 255, 255});
  joinText->setTextScale(1.5);
  // Create join button
  auto joinFn = std::bind(&JoinMenuHudGroup::joinServer, this);
  std::shared_ptr<TextButton> joinElement = std::make_shared<TextButton>(joinText, 0, 60, 100, 50, joinFn, ALIGN_CENTER, ALIGN_CENTER);
  joinElement->setMouseOverColour({200, 200, 200});
  joinElement->setOnClickOffset(-1, 2);
  addElement(joinElement);

  // Create go back button text
  std::shared_ptr<Text> backText = textManager.createText("Aileron-Black", "BACK");
  backText->setTextAlignment(TEXT_ALIGN_LEFT, TEXT_ALIGN_BOTTOM);
  backText->setTextColour({255, 255, 255});
  backText->setTextScale(1.5);
  // Create go back button element
  std::shared_ptr<TextButton> backElement = std::make_shared<TextButton>(backText, 50, -50, 200, 30, goBackFn, ALIGN_LEFT, ALIGN_BOTTOM);
  backElement->setMouseOverColour({200, 200, 200});
  backElement->setOnClickOffset(-1, 2);
  addElement(backElement);

  // Create loading text
  std::shared_ptr<Text> loadingText = textManager.createText("Aileron-Black", "LOADING...");
  loadingText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
  loadingText->setTextColour({255, 255, 255});
  loadingText->setTextScale(3.);
    // Create loading text element
  std::shared_ptr<TextHudElement> loadingElement = std::make_shared<TextHudElement>(loadingText, 0, 0, 400, 80, ALIGN_CENTER, ALIGN_CENTER);
  loadingElement->setIsPostProcessed(false);
  loadingElement->setIsVisible(false);
  mLoadingText = loadingElement;
  addElement(loadingElement);
}

void JoinMenuHudGroup::update() {
  AbstractHudGroup::update();

  if (mJoinServer) {
    // Get nickname in text field
    std::shared_ptr<InputField> nicknameField = mNicknameField.lock();
    std::string nickname = nicknameField->mText->getText();

    // Get address in text field
    std::shared_ptr<InputField> addressField = mAddressField.lock();
    std::string address = addressField->mText->getText();

    // Set nickname
    handle_system_command({"nickname", nickname});

    // Attempt connection
    bool retVal = handle_system_command({"open", address});

    if (retVal) {
      // If successful move to bomberbloke scene
      _pNewScene = std::make_shared<BomberBlokeScene>(10, 10);
    } else {
      // If failed go back to main menu
      showJoinMenu();
      mJoinServer = false;
    }
  }
}

void JoinMenuHudGroup::joinServer() {
  showLoadingText();
  mJoinServer = true;
}

void JoinMenuHudGroup::showLoadingText() {
  // Make all HUD elements invisible and non-interactive
  for (auto i = mHudElements.begin(); i != mHudElements.end(); i++) {
    (*i)->setIsVisible(false);
    (*i)->mIsInteractive = false;
  }

  // Set loading text visible
  std::shared_ptr<TextHudElement> loadingText = mLoadingText.lock();
  loadingText->setIsVisible(true);
}

void JoinMenuHudGroup::showJoinMenu() {
  // Make all HUD elements visible and interactive
  for (auto i = mHudElements.begin(); i != mHudElements.end(); i++) {
    (*i)->setIsVisible(true);
    (*i)->mIsInteractive = true;
  }

  // Set loading text visible
  std::shared_ptr<TextHudElement> loadingText = mLoadingText.lock();
  loadingText->setIsVisible(false);
}

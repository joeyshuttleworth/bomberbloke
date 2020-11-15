#include "EndRoundHudGroup.hpp"

#include "TextHudElement.hpp"
#include "AbstractPlayer.hpp"
#include "engine.hpp"

bool comparePlayers(std::shared_ptr<AbstractPlayer> player1, std::shared_ptr<AbstractPlayer> player2) {
  if (player1->getWins() >= player2->getWins()) {
    return true;
  } else {
    return false;
  }
}

EndRoundHudGroup::EndRoundHudGroup()
  : AbstractHudGroup(0, 0) {
  // Create round winner label
  std::shared_ptr<Text> winnerLabelText = textManager.createText("Aileron-Black", "ROUND WINNER");
  winnerLabelText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
  winnerLabelText->setTextColour({255, 255, 255});
  winnerLabelText->setTextScale(1.5);
  std::shared_ptr<TextHudElement> winnerLabelElement = std::make_shared<TextHudElement>(winnerLabelText, 0, -200, 300, 50, ALIGN_CENTER, ALIGN_CENTER);
  addElement(winnerLabelElement);

  // Create round winner text
  std::shared_ptr<Text> winnerText = textManager.createText("Aileron-Black", "");
  winnerText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
  winnerText->setTextColour({255, 255, 255});
  winnerText->setTextScale(4.);
  std::shared_ptr<TextHudElement> winnerElement = std::make_shared<TextHudElement>(winnerText, 0, -100, 500, 150, ALIGN_CENTER, ALIGN_CENTER);
  addElement(winnerElement);
  mWinnerText = winnerElement;

  setIsPostProcessed(true);
}

void EndRoundHudGroup::updateScores(std::string roundWinner, std::list<std::shared_ptr<AbstractPlayer>> playerList) {
  std::shared_ptr<TextHudElement> winnerText = mWinnerText.lock();

  winnerText->setText(roundWinner);

  // std::cout << playerList.size() << " update scores " << mScoreTexts.size() << std::endl;

  std::list<std::shared_ptr<AbstractPlayer>> sortedPlayerList = playerList;
  sortedPlayerList.sort(comparePlayers);
  auto playerIter = sortedPlayerList.begin();
  auto textIter = mScoreTexts.begin();
  // std::cout << sortedPlayerList.size() << " ff scores " << mScoreTexts.size() << std::endl;
  for (int i = 0; i < sortedPlayerList.size(); i++) {
    if (i >= mScoreTexts.size()) {
      std::shared_ptr<Text> playerText = textManager.createText("Aileron-Black", (*playerIter)->mNickname);
      playerText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
      playerText->setTextColour({255, 255, 255});
      playerText->setTextScale(1.5);
      std::shared_ptr<TextHudElement> playerElement = std::make_shared<TextHudElement>(playerText, -100, i * 50 + 25, 250, 50, ALIGN_CENTER, ALIGN_CENTER);
      addElement(playerElement);

      std::shared_ptr<Text> scoreText = textManager.createText("Aileron-Black", std::to_string((*playerIter)->getWins()));
      scoreText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
      scoreText->setTextColour({255, 255, 255});
      scoreText->setTextScale(1.5);
      std::shared_ptr<TextHudElement> scoreElement = std::make_shared<TextHudElement>(scoreText, -300, i * 50 + 25, 50, 50, ALIGN_CENTER, ALIGN_CENTER);
      addElement(scoreElement);

      mScoreTexts.push_back(std::make_pair(playerElement, scoreElement));
    } else {
      std::shared_ptr<TextHudElement> playerElement = textIter->first.lock();
      std::shared_ptr<TextHudElement> scoreElement = textIter->second.lock();
      playerElement->setText((*playerIter)->mNickname);
      scoreElement->setText(std::to_string((*playerIter)->getWins()));
      textIter++;
    }
    playerIter++;
  }

  if (sortedPlayerList.size() < mScoreTexts.size()) {
    auto textIter = mScoreTexts.begin();
    std::advance(textIter, sortedPlayerList.size());
    for (; textIter != mScoreTexts.end(); textIter++) {
      std::shared_ptr<TextHudElement> playerElement = textIter->first.lock();
      std::shared_ptr<TextHudElement> scoreElement = textIter->second.lock();
      playerElement->setText("");
      scoreElement->setText("");
    }
  }
}

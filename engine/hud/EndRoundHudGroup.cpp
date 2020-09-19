#include "EndRoundHudGroup.hpp"

#include "TextHudElement.hpp"
#include "engine.hpp"

EndRoundHudGroup::EndRoundHudGroup()
  : AbstractHudGroup(0, 0) {
  // Create round winner label
  std::shared_ptr<Text> winnerLabelText = textManager.createText("Aileron-Black", "ROUND WINNER");
  winnerLabelText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
  winnerLabelText->setTextColour({255, 255, 255});
  winnerLabelText->setTextScale(1.5);
  std::shared_ptr<TextHudElement> winnerLabelElement = std::make_shared<TextHudElement>(winnerLabelText, 0, -200, 100, 50, ALIGN_CENTER, ALIGN_CENTER);
  addElement(winnerLabelElement);

  // Create round winner text
  std::shared_ptr<Text> winnerText = textManager.createText("Aileron-Black", "");
  winnerText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
  winnerText->setTextColour({255, 255, 255});
  winnerText->setTextScale(4.);
  std::shared_ptr<TextHudElement> winnerElement = std::make_shared<TextHudElement>(winnerLabelText, 0, -100, 500, 150, ALIGN_CENTER, ALIGN_CENTER);
  addElement(winnerElement);
  mWinnerText = winnerElement;

  setIsPostProcessed(true);
}

void EndRoundHudGroup::updateScores(std::string roundWinner, std::list<std::pair<std::string, int>> scores) {
  std::shared_ptr<TextHudElement> winnerText = mWinnerText.lock();
  winnerText->setText(roundWinner);

  auto scoreIter = scores.begin();
  auto textIter = mScoreTexts.begin();
  for (int i = 0; i < scores.size(); i++) {
    if (i >= mScoreTexts.size()) {
      std::shared_ptr<Text> playerText = textManager.createText("Aileron-Black", scoreIter->first);
      playerText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
      playerText->setTextColour({255, 255, 255});
      playerText->setTextScale(1.5);
      std::shared_ptr<TextHudElement> playerElement = std::make_shared<TextHudElement>(playerText, -100, i * 50 + 25, 250, 50, ALIGN_CENTER, ALIGN_CENTER);
      addElement(playerElement);

      std::shared_ptr<Text> scoreText = textManager.createText("Aileron-Black", std::to_string(scoreIter->second));
      scoreText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
      scoreText->setTextColour({255, 255, 255});
      scoreText->setTextScale(1.5);
      std::shared_ptr<TextHudElement> scoreElement = std::make_shared<TextHudElement>(scoreText, -300, i * 50 + 25, 0, 50, ALIGN_CENTER, ALIGN_CENTER);
      addElement(scoreElement);

      mScoreTexts.push_back(std::make_pair(playerElement, scoreElement));
    } else {
      std::shared_ptr<TextHudElement> playerElement = textIter->first.lock();
      std::shared_ptr<TextHudElement> scoreElement = textIter->second.lock();
      playerElement->setText(scoreIter->first);
      scoreElement->setText(std::to_string(scoreIter->second));
      scoreIter++;
    }
    textIter++;
  }
}

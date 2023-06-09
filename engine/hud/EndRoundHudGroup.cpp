#include "EndRoundHudGroup.hpp"

#include "AbstractPlayer.hpp"
#include "TextHudElement.hpp"
#include "engine.hpp"

bool
comparePlayers(std::shared_ptr<AbstractPlayer> player1,
               std::shared_ptr<AbstractPlayer> player2)
{
  if (player1->getWins() >= player2->getWins()) {
    return true;
  } else {
    return false;
  }
}

EndRoundHudGroup::EndRoundHudGroup()
  : AbstractHudGroup(0, 0)
{
  // Create round winner label
  std::shared_ptr<Text> winnerLabelText =
    textManager.createText("Aileron-Black", "ROUND WINNER");
  winnerLabelText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
  winnerLabelText->setTextColour({ 255, 255, 255, 255 });
  winnerLabelText->setTextScale(1.5);
  std::shared_ptr<TextHudElement> winnerLabelElement =
    std::make_shared<TextHudElement>(
      winnerLabelText, 0, -200, 300, 50, ALIGN_CENTER, ALIGN_CENTER);
  addElement(winnerLabelElement);

  // Create round winner text
  std::shared_ptr<Text> winnerText =
    textManager.createText("Aileron-Black", "");
  winnerText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
  winnerText->setTextColour({ 255, 255, 255, 255 });
  winnerText->setTextScale(4.);
  std::shared_ptr<TextHudElement> winnerElement =
    std::make_shared<TextHudElement>(
      winnerText, 0, -100, 500, 150, ALIGN_CENTER, ALIGN_CENTER);
  addElement(winnerElement);
  mWinnerText = winnerElement;

  setIsPostProcessed(true);
}


std::pair<std::shared_ptr<TextHudElement>, std::shared_ptr<TextHudElement>>
EndRoundHudGroup::createBlankScoreText(int index)
{
  std::shared_ptr<Text> playerText =
    textManager.createText("Aileron-Black", "");
  playerText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
  playerText->setTextColour({ 255, 255, 255, 255 });
  playerText->setTextScale(1.5);
  std::shared_ptr<TextHudElement> playerElement =
    std::make_shared<TextHudElement>(
      playerText, -100, index * 50 + 25, 250, 50, ALIGN_CENTER, ALIGN_CENTER);
  addElement(playerElement);

   std::shared_ptr<Text> scoreText =
    textManager.createText("Aileron-Black", "");
   scoreText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
   scoreText->setTextColour({ 255, 255, 255, 255 });
   scoreText->setTextScale(1.5);
   std::shared_ptr<TextHudElement> scoreElement =
     std::make_shared<TextHudElement>(
       scoreText, -300, index * 50 + 25, 50, 50, ALIGN_CENTER, ALIGN_CENTER);
   addElement(scoreElement);

   return std::make_pair(playerElement, scoreElement);
}

void
EndRoundHudGroup::updateScores(
  std::string roundWinner,
  std::list<std::shared_ptr<AbstractPlayer>> playerList)
{
  // Set the winner
  std::shared_ptr<TextHudElement> winnerText = mWinnerText.lock();
  winnerText->setText(roundWinner);

  // Clear stale scores
  for(auto & mScoreText : mScoreTexts) {
    std::shared_ptr<TextHudElement> playerElement = mScoreText.first.lock();
    std::shared_ptr<TextHudElement> scoreElement = mScoreText.second.lock();
    playerElement->setText("");
    scoreElement->setText("");
  }

  // Create new fields if needs be
  for(int i = (int) playerList.size(); playerList.size() > mScoreTexts.size(); i++) {
    printf("Had to create a new score text\n");
    auto scoreText = createBlankScoreText(i);
    mScoreTexts.push_back(scoreText);
  }

  // Update score texts
  std::list<std::shared_ptr<AbstractPlayer>> sortedPlayerList = playerList;
  sortedPlayerList.sort(comparePlayers);
  auto playerIter = sortedPlayerList.begin();
  auto textIter = mScoreTexts.begin();
  while(playerIter != sortedPlayerList.end()) {
    std::shared_ptr<TextHudElement> playerElement = textIter->first.lock();
    std::shared_ptr<TextHudElement> scoreElement = textIter->second.lock();

    int ping = (*playerIter)->mMetadata.numeric["lastPingMeasurement"];
    std::string nickname = (*playerIter)->mNickname;
    playerElement->setText(std::to_string(ping) + " - " + nickname);

    scoreElement->setText(std::to_string((*playerIter)->getWins()));

    playerIter++;
    textIter++;
  }
}
//
//void
//EndRoundHudGroup::updateScores(
//  std::string roundWinner,
//  std::list<std::shared_ptr<AbstractPlayer>> playerList)
//{
//  std::shared_ptr<TextHudElement> winnerText = mWinnerText.lock();
//
//  winnerText->setText(roundWinner);
//
//  // std::cout << playerList.size() << " update scores " << mScoreTexts.size()
//  // << std::endl;
//
//  std::list<std::shared_ptr<AbstractPlayer>> sortedPlayerList = playerList;
//  sortedPlayerList.sort(comparePlayers);
//  auto playerIter = sortedPlayerList.begin();
//  auto textIter = mScoreTexts.begin();
//  // std::cout << sortedPlayerList.size() << " ff scores " << mScoreTexts.size()
//  // << std::endl;
//  for (unsigned int i = 0; i < sortedPlayerList.size(); i++) {
//    if (i >= mScoreTexts.size()) {
//      int ping = (*playerIter)->getPlayerProperties()->mLastPingMeasurement;
//      std::string nickname = (*playerIter)->mNickname;
//      std::shared_ptr<Text> playerText =
//        textManager.createText("Aileron-Black", std::to_string(ping) + " - ");
//      playerText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
//      playerText->setTextColour({ 255, 255, 255, 255 });
//      playerText->setTextScale(1.5);
//      std::shared_ptr<TextHudElement> playerElement =
//        std::make_shared<TextHudElement>(
//          playerText, -100, i * 50 + 25, 250, 50, ALIGN_CENTER, ALIGN_CENTER);
//      addElement(playerElement);
//
//      std::shared_ptr<Text> scoreText = textManager.createText(
//        "Aileron-Black", std::to_string((*playerIter)->getWins()));
//      scoreText->setTextAlignment(TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
//      scoreText->setTextColour({ 255, 255, 255, 255 });
//      scoreText->setTextScale(1.5);
//      std::shared_ptr<TextHudElement> scoreElement =
//        std::make_shared<TextHudElement>(
//          scoreText, -300, i * 50 + 25, 50, 50, ALIGN_CENTER, ALIGN_CENTER);
//      addElement(scoreElement);
//
//      mScoreTexts.push_back(std::make_pair(playerElement, scoreElement));
//    } else {
//      std::shared_ptr<TextHudElement> playerElement = textIter->first.lock();
//      std::shared_ptr<TextHudElement> scoreElement = textIter->second.lock();
//      playerElement->setText((*playerIter)->mNickname);
//      scoreElement->setText(std::to_string((*playerIter)->getWins()));
//      textIter++;
//    }
//    playerIter++;
//  }
//
//  if (sortedPlayerList.size() < mScoreTexts.size()) {
//    auto textIter = mScoreTexts.begin();
//    std::advance(textIter, sortedPlayerList.size());
//    for (; textIter != mScoreTexts.end(); textIter++) {
//      std::shared_ptr<TextHudElement> playerElement = textIter->first.lock();
//      std::shared_ptr<TextHudElement> scoreElement = textIter->second.lock();
//      playerElement->setText("");
//      scoreElement->setText("");
//    }
//  }
//}

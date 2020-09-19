#ifndef ENDROUNDHUDGROUP_HPP
#define ENDROUNDHUDGROUP_HPP

#include "AbstractHudGroup.hpp"

#include <memory>
#include <string>
#include <utility>

#include "AbstractPlayer.hpp"

class TextHudElement;

class EndRoundHudGroup: public AbstractHudGroup {
public:
  /**
   * Constructor for EndRoundHudGroup.
   */
  EndRoundHudGroup();

  /**
   * Update scores presented by HUD group.
   *
   * @param roundWinner Player that won the round.
   * @param playerList  List of players.
   */
  void updateScores(std::shared_ptr<AbstractPlayer> roundWinner, std::list<std::shared_ptr<AbstractPlayer>> playerList);

protected:
  // Weak pointer containing nickname of round winner.
  std::weak_ptr<TextHudElement> mWinnerText;

  // List of score HUD elements. First in pair is nickname, second is score value.
  std::list<std::pair<std::weak_ptr<TextHudElement>, std::weak_ptr<TextHudElement>>> mScoreTexts;
};

#endif

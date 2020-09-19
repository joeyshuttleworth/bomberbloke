#ifndef ENDROUNDHUDGROUP_HPP
#define ENDROUNDHUDGROUP_HPP

#include "AbstractHudGroup.hpp"

#include <memory>
#include <string>
#include <utility>

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
   * @param roundWinner Nickname of round winner.
   * @param scores      List of pairs with nicknames and scores.
   */
  void updateScores(std::string roundWinner, std::list<std::pair<std::string, int>> scores);

protected:
  // Weak pointer containing nickname of round winner.
  std::weak_ptr<TextHudElement> mWinnerText;

  // List of score HUD elements. First in pair is nickname, second is score value.
  std::list<std::pair<std::weak_ptr<TextHudElement>, std::weak_ptr<TextHudElement>>> mScoreTexts;
};

#endif

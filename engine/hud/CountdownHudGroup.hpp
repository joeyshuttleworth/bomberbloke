#ifndef COUNTDOWNHUDGROUP_HPP
#define COUNTDOWNHUDGROUP_HPP

#include "AbstractHudGroup.hpp"

#include <functional>
#include <memory>

class Text;
class TextHudElement;
class TextManager;
class SoundManager;
class Sound;

extern TextManager textManager;
extern SoundManager soundManager;

class CountdownHudGroup: public AbstractHudGroup {
protected:
  // Number of ticks left in countdown.
  int mTicksLeft = 0;

  // Maximum amount of glow applied to the text.
  int mMaxGlowAmount;

  // Weak pointer to countdown text HUD element
  std::weak_ptr<TextHudElement> mCountdownText;

  // Callback function for when the countdown is finished.
  std::function<void()> mOnFinished;

  // Countdown sound effects
  std::shared_ptr<Sound> m3Sound;
  std::shared_ptr<Sound> m2Sound;
  std::shared_ptr<Sound> m1Sound;
  std::shared_ptr<Sound> mCommenceSound;
public:
  /**
   * Constructor for CountdownHudGroup.
   *
   * @param onFinished    Callback function for when the countdown is finished.
   * @param maxGlowAmount Amount of glow applied to countdown text.
   */
  CountdownHudGroup(std::function<void()> onFinished, int maxGlowAmount=0);

  /**
   * Starts the countdown.
   *
   * @param nSecs Number of seconds the countdown lasts.
   */
  void start(int nSecs);

  /**
   * Update method to be called on every tick.
   */
  void update() override;
};

#endif

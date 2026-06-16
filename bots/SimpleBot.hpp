#ifndef SIMPLEBOT_HPP
#define SIMPLEBOT_HPP

#include <fstream>
#include "AbstractBot.hpp"

class SimpleBot : public AbstractBot{
protected:
  Uint8 mState = 0;
  unsigned int mPollDelay = 120;

  int getActorId();

public:
  virtual void handleReset();
  virtual void update(unsigned int);

  virtual ~SimpleBot();

  using AbstractBot::AbstractBot;
};

#endif

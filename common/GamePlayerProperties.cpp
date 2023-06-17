#include "GamePlayerProperties.hpp"
#include "CommandEvent.hpp"
#include "bloke.hpp"

GamePlayerProperties::GamePlayerProperties(std::shared_ptr<bloke> b)
{
  mSpeed = b->mMaxSpeed;
  mMaxBombs = b->mMaxBombs;
  mPower = b->mPower;
  if (!mBigBomb && b->mBigBomb) {
    _pScene->handleCommand("bigbomb");
    if (_server) {
      std::unique_ptr<AbstractEvent> command(new CommandEvent("bigbomb"));
      if (auto p = b->getPlayer())
        _net_server.mConnector->sendEvent(command, p->getId());
    }
  }
  mBigBomb = b->mBigBomb;
}

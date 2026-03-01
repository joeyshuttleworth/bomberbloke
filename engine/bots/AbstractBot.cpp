#include "AbstractBot.hpp"

AbstractBot::~AbstractBot(){}

AbstractBot::AbstractBot(std::string nickname, std::shared_ptr<scene> _pScene=nullptr, std::shared_ptr<actor> _pActor = nullptr,
                         std::unique_ptr<NetClient> net_client=nullptr){
  // Use 0 as initial state
  mState = 0;
  mpActor = _pActor;
  mpNetClient = std::move(net_client);
  mpScene = _pScene;
  mNickname = nickname;
};


void AbstractBot::connectScene(std::shared_ptr<scene> pScene){
  mpScene = pScene;
}

#include "AbstractBot.hpp"
#include "serverPlayer.hpp"
#include "actor.hpp"

AbstractBot::~AbstractBot(){}

AbstractBot::AbstractBot(std::string nickname, std::shared_ptr<scene> _pScene=nullptr, std::shared_ptr<actor> _pActor = nullptr,
                         std::unique_ptr<NetClient> net_client=nullptr) :
  mState(0),
  mNickname(nickname),
  mpScene(_pScene),
  mpActor(_pActor),
  mpNetClient(std::move(net_client))
{
};


void AbstractBot::connectScene(std::shared_ptr<scene> pScene){
  mpScene = pScene;
}

int AbstractBot::getActorId(){
  // Search by unique nickname

  std::string nickname = getNickname();

  std::vector<serverPlayer> server_players = mpNetClient->getPlayers();
  auto iter = std::find_if(
                           server_players.begin(),
                           server_players.end(),
                           [&](auto sp) -> bool {return sp.isLocal() && (sp.getNickname() == nickname);}
                           );

  if(iter == server_players.end()){
    // We're not registered as a player, so we can't be connected to an actor
    log_message(ERR, "Bot disconnected from server");
    return -1;
  }

  auto player_id = iter->getId();

  auto actor_iter = std::find_if(
                                 _pScene->mActors.begin(),
                                 _pScene->mActors.end(),
                                 [&](std::shared_ptr<actor> a) -> bool {return a->getPlayerId() == player_id;}
                           );

  if(actor_iter!=_pScene->mActors.end()){
    return (*actor_iter)->getId();
  }

  log_message(DEBUG, "No actor connected to bot");

  return -1;
}


#include <random>
#include "LocalPlayer.hpp"
#include "SimpleBot.hpp"
#include "CommandEvent.hpp"
#include "serverPlayer.hpp"
#include "actor.hpp"


int SimpleBot::getActorId(){
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


void SimpleBot::handleReset(){
  mpActor = nullptr;
  mState = 0;

  // Will be -1 if no actor exists. That's fine
  mActorId = getActorId();
}


void SimpleBot::update(unsigned int current_tick){

  if (current_tick - mLastHandledTick < mPollDelay){
    // Delay update until later
    return;
  }

  mLastHandledTick = current_tick;

  // Check we are still alive
  // if (mState != 0){
  //   // We're dead. Nothing to do
  // }

  if(mpScene==nullptr){
    // No scene connected
    log_message(WARNING, "SimpleBot not connected to scene");
    return;
  }

  mActorId = getActorId();
  std::shared_ptr<actor> my_actor = mpScene->GetActor(mActorId);

  if(my_actor == nullptr){
    if(mActorId < 0){
      // We've are dead. Update state
      // mState = 1;
      return;
    }
    else{
      my_actor = mpScene->GetActor(mActorId);
    }

  }

  // Choose random action (4 directions or plant bomb)
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist5(1, 5);

  std::string command = "";
  switch(dist5(rng)){
  case 1:
    command = "+up";
    break;
  case 2:
    command = "+right";
    break;
  case 3:
    command = "+down";
    break;
  case 4:
    command = "+left";
    break;
  case 5:
    command = "+bomb";
    break;
  default:
    break;
  }

  log_message(DEBUG, "Command is: " + command + "\n");

  // Update locally
  my_actor->handleCommand(command);

  if(mpNetClient){
    // Send command over network if we're a client
    std::unique_ptr<AbstractEvent> c_event(
                                           new CommandEvent(command));
    mpNetClient->mConnector->broadcastEvent(std::move(c_event));
  }
}

SimpleBot::~SimpleBot(){}


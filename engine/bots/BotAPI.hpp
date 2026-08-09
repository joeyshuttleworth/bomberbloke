#ifndef BOTAPI_HPP
#define BOTAPI_HPP

#include <memory>
#include <vector>
#include <set>

#include "actor.hpp"
#include "AbstractBot.hpp"
#include "engine.hpp"

class AbstractBot;

class BotAPI
{
public:
  BotAPI() = default;
  BotAPI(AbstractBot* bot) : mpBot(bot){}
  BotAPI(BotAPI& other){mpBot=other.mpBot;}
  ~BotAPI() = default;
  BotAPI operator=(BotAPI&) = delete;
  BotAPI& operator=(BotAPI&&) = delete;

  // TODO implement
  std::vector<std::shared_ptr<actor>> getActors(){return {};}

  std::string getSceneJSON();

  void handleCommandTokens(Tokens tokens);
  void handleCommand(std::string command)
  {
    handleCommandTokens(split_to_tokens(command));
  }

  void setBot(AbstractBot* bot)
  {
    mpBot = bot;
  }

  void connectScene(std::shared_ptr<scene> scn){
    if(mpBot)
      mpBot->connectScene(scn);
  }

  NavGrid getNavGrid();
  std::vector<ivector> getConnectedComponentFromNode(NavGrid& nav_grid, ivector node){
    auto component = nav_grid.getConnectedComponentFromNode(node);

    std::vector<std::array<int, 2>> vec(component.begin(), component.end());
    return vec;
  }

  dvector getActorLocation();

  private:
  AbstractBot* mpBot = nullptr;
};

#endif

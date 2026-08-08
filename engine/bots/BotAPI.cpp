#include "BotAPI.hpp"
#include <vector>

void BotAPI::handleCommandTokens(std::vector<std::string> command){
  if(mpBot)
    mpBot->handleCommand(command);
}

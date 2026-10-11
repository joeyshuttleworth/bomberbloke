#ifndef BOTAPI_HPP
#define BOTAPI_HPP

#include <memory>
#include <vector>
#include <set>
#include <array>

#include "BotAPI.hpp"

class AbstractBot;

class BomberBlokeBotAPI : public BotAPI
{
public:
  using BotAPI::BotAPI;

  std::vector<std::array<int, 2>> getDangerousSquares();
};

#endif

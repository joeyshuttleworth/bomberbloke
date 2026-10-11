#include "BomberBlokeBotAPI.hpp"


std::set<std::array<int, 2>> BomberBlokeBotAPI::getDangerousSquares(int timeout)
{
  std::vector<dvector> ret_set = {};

  if(mpBot)
    return ret_set;

  scene = mpBot->getScene();

  if(!scene)
    return ret_set;

  for(const auto& actor : scene->mActors){
    if(actor->getType() == ACTOR_BOMB){
      std::shared_ptr<bomb> bomb = std::static_cast<Bomb>(actor);

      if((timeout == 0) || (bomb->getTicksToDetonation() <= timeout)){
        auto danger_squares = bomb->getAffectedSquares();
        for (const& auto sq : danger_squares)
          ret_set.insert(sq);
      }
    }
  }

  return ret_set
}

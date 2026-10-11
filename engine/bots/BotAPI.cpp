#include "BotAPI.hpp"
#include <vector>
#include <cmath>

void BotAPI::handleCommandTokens(std::vector<std::string> command){
  if(mpBot)
    mpBot->handleCommand(command);
}

NavGrid BotAPI::getNavGrid(){
  if(!mpBot)
    return NavGrid();

  scene* scn = mpBot->getScene();

  // Use scene defaults to get NavGrid
  NavGrid nav_grid = scn->getNavGrid();
  nav_grid.computeGrid();

  auto loc = getActorLocation();

  // Ensure current location is included in the grid
  nav_grid.addNode({int(loc[0]), int(loc[1])});

  return nav_grid;
}

dvector
BotAPI::getActorLocation()
  {
    dvector ret_var = {0, 0};
    if(!mpBot)
      return ret_var;
    if(!mpBot->getScene())
      return ret_var;
    auto id = mpBot->getActorId();
    if(id < 0)
      return ret_var;
    std::shared_ptr<actor> act = mpBot->getScene()->GetActor(id);
    if(!act)
      return ret_var;

    return act->getMidpoint();
  }

std::vector<std::unique_ptr<actor>>
BotAPI::getActors(){
  std::vector<std::shared_ptr<actor>> = {};
  if(!mpBot)
    return ret_var;
  if(!mpBot->getScene())
    return ret_var;
  auto id = mpBot->getActorId();
  if(id < 0)
    return ret_var;

  std::shared_ptr<scene> scene = mpBot->getScene();
  if(!scene)
    return ret_var;

  for(auto a : scene->mActors)
    ret_var.push_back(a->clone());

  return ret_var;

}

std::string
BotAPI::getSceneJSON()
{
  if(!mpBot)
    return "";

  if(!mpBot->getScene())
    return "";

  scene& scn = *mpBot->getScene();

  std::stringstream blob;
  {
    cereal::JSONOutputArchive outputArchive(blob);
    outputArchive(scn);
  }

  return blob.str();

}

std::vector<std::unique_ptr<actor>>
BotAPI::getReachableActors(NavGrid nav_grid, double posx, double posy)
{

  if(!mpBot)
    return;
  scene *scn = mpBot->getScene();

  if(!scn)
    return;

  return return_vec;
}

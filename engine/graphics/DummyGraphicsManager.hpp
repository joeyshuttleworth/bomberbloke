#ifndef DUMMYGRAPHICSMANAGER_HPP
#define DUMMYGRAPHICSMANAGER_HPP

#include <vector>
#include <string>

#include "IGraphicsManager.hpp"
#include "AbstractCamera.hpp"

class Sound;

class DummyGraphicsManager : public IGraphicsManager{
protected:
  /* TODO Add some debugging functionality */
  // std::vector<std::string> mLoadedSpriteNames{};
public:
  void loadSpriteFromPath(std::string){
    // mLoadedSpriteNames.push_back(p);
  }
};

#endif

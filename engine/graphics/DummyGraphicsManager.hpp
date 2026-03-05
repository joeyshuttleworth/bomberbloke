#ifndef DUMMYGRAPHICSMANAGER_HPP
#define DUMMYGRAPHICSMANAGER_HPP

#include <vector>
#include <string>

#include "IGraphicsManager.hpp"
#include "AbstractCamera.hpp"
#include "DummyCamera.hpp"

class DummyGraphicsManager : public IGraphicsManager{
protected:
  std::vector<std::string> mLoadedSpriteNames{};
public:
  void loadSpriteFromPath(std::string p){
    mLoadedSpriteNames.push_back(p);
  }
};

#endif

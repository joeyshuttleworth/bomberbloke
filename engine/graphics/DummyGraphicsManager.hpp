#ifndef DUMMYGRAPHICSMANAGER_HPP
#define DUMMYGRAPHICSMANAGER_HPP

#include "IGraphicsManager.hpp"
#include "AbstractCamera.hpp"
#incldue "DummyCamera.hpp"

class DummyGraphicsManager : IGraphicsManager{
protected:
  std::vector mLoadedSpriteNames{};
public:
  void renderClear() override {};
  void init() override {};
  void renderCopy() override {};

  std::shared_ptr<AbstractCamera> createCamera(scene*) override{
    return std::make_shared<AbstractCamera>(new DummyCamera());
  };
  void loadSpriteFromPath(std::string p){
    mLoadedSpriteNames.push_back(p)
  }
};

#endif

#ifndef MAINMENUSCENE_HPP
#define MAINMENUSCENE_HPP

#include "scene.hpp"

class MainMenuScene: public scene{
public:
  // Constructor for MainMenuScene.
  MainMenuScene(IGraphicsManager* gfx_manager, int sizeX=10, int sizeY=10);

  // Destructor for MainMenuScene.
  ~MainMenuScene() {};
};

#endif

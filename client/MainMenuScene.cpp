#include "MainMenuScene.hpp"

#include <limits>

#include "Camera.hpp"
#include "Explosion.hpp"
#include "MainMenuHudGroup.hpp"

MainMenuScene::MainMenuScene(int sizeX, int sizeY)
{
  std::shared_ptr<MainMenuHudGroup> menuHud =
    std::make_shared<MainMenuHudGroup>();
  mHudElements.push_back(menuHud);

  for (int i = 0; i < sizeX; i++) {
    for (int j = 0; j < sizeY; j++) {
      int speed = (30 + i + 2 * j);
      std::shared_ptr<Explosion> explosion = std::make_shared<Explosion>(
        i, j, 1, 1, speed, std::numeric_limits<int>::max(), 0, false, false, 0);
      mParticleList.push_back(explosion);
    }
  }

  mpCamera->mPosition[0] = ((double)sizeX) / 2;
  mpCamera->mPosition[1] = ((double)sizeY) / 2;
  mpCamera->mZoom = 1.2 / std::max(sizeX, sizeY);

  mpCamera->setBlur(20);
  mpCamera->setBrightness(-80);
}

#include "MainMenuScene.hpp"

#include <limits>

#include "MainMenuHudGroup.hpp"
#include "Explosion.hpp"
#include "Camera.hpp"

MainMenuScene::MainMenuScene(int sizeX, int sizeY)
  : scene(sizeX, sizeY) {
  std::shared_ptr<MainMenuHudGroup> menuHud = std::make_shared<MainMenuHudGroup>();
  mHudElements.push_back(menuHud);

  for (int i = 0; i < sizeX; i++) {
    for (int j = 0; j < sizeY; j++) {
      int speed = (30 + i + 2*j);
      std::shared_ptr<Explosion> explosion = std::make_shared<Explosion>(i, j, 1, 1, speed, std::numeric_limits<int>::max(), 0, false, false, 0);
      mParticleList.push_back(explosion);
    }
  }

  mpCamera->mPosition[0] = ((double) mDimmension[0]) / 2;
  mpCamera->mPosition[1] = ((double) mDimmension[1]) / 2;
  double ratio = mpCamera->getScreenDimensions()[0] / mpCamera->getScreenDimensions()[1];
  mpCamera->mZoom = 1.2 / std::fmax(mDimmension[0], mDimmension[1] * ratio);

  mpCamera->setBlur(20);
  mpCamera->setBrightness(-80);
}

void MainMenuScene::onResize() {
  scene::onResize();
  mpCamera->mPosition[0] = ((double) mDimmension[0]) / 2;
  mpCamera->mPosition[1] = ((double) mDimmension[1]) / 2;
  double ratio = mpCamera->getScreenDimensions()[0] / mpCamera->getScreenDimensions()[1];
  mpCamera->mZoom = 1.2 / std::fmax(mDimmension[0], mDimmension[1] * ratio);
}

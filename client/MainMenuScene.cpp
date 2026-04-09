#include "MainMenuScene.hpp"
#include <limits>
#include "Explosion.hpp"
#include "MainMenuHudGroup.hpp"
#include "ShowAllCamera.hpp"

MainMenuScene::MainMenuScene(IGraphicsManager* gfx_manager, int size_x, int size_y) : scene(gfx_manager, size_x, size_y)
{

  if(!gfx_manager)
    return;

  std::shared_ptr<MainMenuHudGroup> menuHud =
    std::make_shared<MainMenuHudGroup>(*this);
  mHudElements.push_back(menuHud);

  mpCamera = std::make_shared<ShowAllCamera>(gfx_manager, this);
  SetCamera(mpCamera);

  for (int i = 0; i < size_x; i++) {
    for (int j = 0; j < size_y; j++) {
      int speed = (30 + i + 2 * j);
      std::shared_ptr<Explosion> explosion =
        std::make_shared<Explosion>(
                                    gfx_manager, i, j, 1, 1, true, speed,
                                    std::numeric_limits<int>::max(), 0, false, false, 0
                                    );

      mParticles.push_back(explosion);
    }
  }

  if(!mpCamera){
    return;
  }

  mpCamera->mPosition[0] = ((double)size_x) / 2;
  mpCamera->mPosition[1] = ((double)size_y) / 2;
  mpCamera->mZoom = 1.2 / std::max(size_x, size_y);

  // gfx_manager->setBlur(20);
  gfx_manager->setBrightness(-80);
}

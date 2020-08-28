#ifndef BOMBERBLOKESCENE_HPP
#define BOMBERBLOKESCENE_HPP
#include "ClickableHudElement.hpp"
#include "engine.hpp"
#include "scene.hpp"
#include "bomberbloke.h"

class BomberBlokeScene : public scene{
protected:
  std::weak_ptr<AbstractHudElement> mPowerIcons[5];
  std::weak_ptr<AbstractHudElement> mSpeedIcons[5];

public:

    BomberBlokeScene(int size_x = 10, int size_y = 10);
    ~BomberBlokeScene(){}

   /**
     * Every second, check how many players are alive.
     * If no players remain, everybody draws. Otherwise,
     * if one player remains, this player wins the match.
     *
     */
    void logicUpdate();

    void draw();
    template<class Archive>
    void serialize(Archive &archive){
      archive(cereal::base_class<scene>(this));
      return;
    }
};

CEREAL_REGISTER_TYPE(BomberBlokeScene)
#endif
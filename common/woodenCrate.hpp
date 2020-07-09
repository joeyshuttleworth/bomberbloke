#ifndef WOODENCRATE_HPP
#define WOODENCRATE_HPP
#include "staticSprite.hpp"
#include "bomberbloke.h"

class woodenCrate : public actor{
public:

  int getType() const{
    return ACTOR_WOODEN_CRATE;
  }

  woodenCrate(int x=0, int y=0) : actor(double(x), double(y), true){
  mpSpriteHandler = std::shared_ptr<staticSprite>(new staticSprite(double(x), double(y), 1.0, 1.0, "crate.png"));
    return;
  }

  void handle_command(std::string command){
    std::list<std::string> tokens = split_to_tokens(command);
    if(command.size() != 1)
      return;
    else if(tokens.front() == "kill" || tokens.front() == "+kill"){
      /* TODO drop powerups and draw particle effect */
      mRemove = true;
    }
  }

  template<class Archive>
  void serialize(Archive &archive){
    archive(cereal::base_class<actor>(this));
    return;
  }
};

CEREAL_REGISTER_TYPE(woodenCrate)
#endif

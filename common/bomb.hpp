#include <cereal/types/base_class.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/polymorphic.hpp>

class bomb : public actor {
 protected:
  Uint8 mTimer;
  Uint8 mPower;
  bloke *mpPlacedBy;
  bool mPenetration;
  bool mBigBomb;
  bool mSatellite;
 public:

  /*Cereal serialisation*/

  template<class Archive>
  void serialize(Archive &archive){
    archive(mPower, mTimer, mSatellite, mBigBomb);
    return;
  }

  void init(bloke*);
  void explode();
  void update();
  void handle_command(std::string command);
  // ~bomb(return;);
  // bomb(){return;};
  using actor::actor;
};

/*This is required for classes using polymorphism*/
CEREAL_REGISTER_TYPE(bomb);

class bomb : public actor {
 protected:
  Uint8 mTimer;
  Uint8 mPower;
  bloke *mpPlacedBy;
  bool mPenetration;
  bool mBig_bomb;
  bool mSatellite;
 public:
  void init(bloke*);
  void explode();
  void update();
  void handle_command(std::string command);
  ~bomb();
  bomb();
  using actor::actor;
};

class bloke : public actor{
 private:
  std::list<int> mPowerups;
  enum DIR{
    DIR_UP,
    DIR_RIGHT,
    DIR_DOWN,
    DIR_LEFT,
    DIR_NONE
  };
  int mDirection;
  void place_bomb();
  double mMaxSpeed = double(DEFAULT_MAX_SPEED);
  Uint8 mMaxBombs = 1;
  bool mAccelerated;
  bool mDirectionsHeld[4] = {false, false, false, false};
  double mAcceleration[2] = {0,0};
 public:
  bloke();
  void  draw();
  void  kick(bomb *bomb, uint8_t direction);
  void  die();

  using actor::actor;

  void handle_command(std::string command);
  void accelerate();
  void update();
  void init();

  int getPower(){
    return mPower;
  }
  int  mBombs = 0;
  int  mPower = 1;

};


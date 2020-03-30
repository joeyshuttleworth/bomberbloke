class bloke : public actor{
 private:
  std::list<int> mPowerups;

  void place_bomb();
  double mMaxSpeed = double(DEFAULT_MAX_SPEED);
  Uint8 mMaxBombs = 1;
  int direction = DIR_NONE;
 public:
  bloke();
  void  draw();
  void  kick(bomb *bomb, uint8_t direction);
  void  die();

  using actor::actor;

  void handle_command(std::string command);
  void accelerate(int);
  void update();
  void init();

  int getPower(){
    return mPower;
  }

  bool mAccelerated = false;

  int  mBombs = 0;
  int  mPower = 1;

};


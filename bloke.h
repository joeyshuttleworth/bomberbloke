class bloke : public actor{
 private:
  std::list<int> powerups;
  bool accelerated = false;
  void place_bomb();
 public:
  bloke();
  void  draw();
  void  kick(bomb *bomb, uint8_t direction);
  void  die();
  SDL_Surface *surf;
  using actor::actor;
  void handle_command(std::string command);
  void accelerate(int);
  double max_speed = double(DEFAULT_MAX_SPEED) / double(TICK_RATE);
  Uint8 max_bombs = 1;
  Uint8 bombs = 0;
  Uint8 power = 1;
  void update();
  void init();
};


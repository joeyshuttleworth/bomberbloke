class bomb : public actor {
  Uint8 timer;
  Uint8 power;
  bloke *placed_by;
  bool penetration;
  bool big_bomb;
  bool satellite;
 public:
  void init(bloke*);
  void explode();
  void update();
  void handle_command(std::string command);
  ~bomb();
  bomb();
  using actor::actor;
};

class actor{
 protected:
 public:
  actor(level*, double, double);
  actor();
  SDL_Surface *sprite = NULL; //protected
  int id;
  double position[2];
  double velocity[2];
  double dim[2];
  level *current_level;
  bool collides;
  void draw();
  int move(double x, double y);
  bool is_moving();
  int init(level*, double, double);
  int direction();
  double *get_midpoint();
  virtual void handle_command(std::string);
  virtual void update();
};

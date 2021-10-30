class player;
class actor
{
protected:
public:
  actor(double, double);
  actor();
  SDL_Texture* sprite = NULL; // should be protected
  void draw();
  int move(double x, double y);
  bool is_moving();
  int init(double, double);
  int direction();
  double* get_midpoint();
  player* controller = NULL;
  virtual ~actor();
  virtual void handle_command(std::string);
  virtual void update();
  Uint32 id;
  bool remove;
  double position[2];
  double velocity[2];
  double dim[2];
  Uint16 type = 0;
  bool collides;
};

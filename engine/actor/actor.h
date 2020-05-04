class player;
class actor{
 protected:
  SDL_Texture *mpSprite = NULL; //should be protected

  player *mpController = NULL;
  Uint32 id;
  
  /*Pointer to the level we're in*/
  level *mpLevel;
 public:
  actor(double, double);
  actor();

  /*Do we collide with other actors*/
  bool mCollides;

  
  /*Position is the bottom left hand side of the actor */  
  double mPosition[2];
  double mDimmension[2];
  double mVelocity[2];

  bool mRemove;
  
  void draw();
  int move(double x, double y);
  bool is_moving();
  int init(double, double);
  int direction();
  double *get_midpoint();
  virtual void update();

  virtual void handle_command(std::string) = 0;

  player *getController(){
    return mpController;
  };
  
  void setController(player *controller){
    mpController = controller;
    return;
  };
  
};

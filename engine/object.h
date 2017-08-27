

class actor{
  void move(double x, double y);
  bool is_block_safe(int x, int y);
  double pos[2];
  double velocity[2];
  double dimmension[2];
  SDL_Surface *sprite;
};

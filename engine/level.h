#include <list>
#include <string>
#include <SDL2/SDL.h>

extern double _zoom;

class actor;

class level{
 private:
  std::string name;
  std::string description;
 public:
  std::vector<int> spawn_points;
  SDL_Texture *sprite=NULL;
  std::list<actor> actor_list;
  uint8_t *blocks=NULL;
  int dim[2];
  level(int x, int y);
  level();
  level(const level &lvl);
  ~level();
  uint8_t get_block(int, int);
  bool is_block_empty(actor *actor , int x, int y);
  void draw();
  void init();
};

extern level _level;

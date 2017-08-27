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
  SDL_Surface *sprite;
  std::list<actor*> actor_list;
  uint8_t *blocks;
  int dim[2];
  level(int x, int y);
  ~level();
  uint8_t get_block(int, int);
  bool is_block_empty(actor *actor , int x, int y);
  void draw();
};

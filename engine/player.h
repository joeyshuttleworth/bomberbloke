#include <list>
#include <sys/socket.h>
#include <unistd.h>
#include <string>

class player{
public:
  ~player();
  player(std::string);
  player();
  uint8_t id;
  std::string nickname;
  int   *get_input();
  int   type;
  virtual void ping();
  actor *character = NULL;
  void init(actor*);
  unsigned int last_ping_time = 0;
  unsigned int last_ping = 0;
};

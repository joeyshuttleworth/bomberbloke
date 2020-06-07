#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <list>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <memory> 
class player{
public:
  ~player();
  player(std::string);
  player();
  int id;
  std::string nickname;
  int   *get_input();
  int   type;
  virtual void ping();
  std::shared_ptr<actor> character = NULL;
  void init(actor*);
  unsigned int last_ping_time = 0;
  unsigned int last_ping = 0;
};
#endif

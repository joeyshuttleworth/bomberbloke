#include <list>
#include <sys/socket.h>
#include <unistd.h>
#include <string>

class player{
public:
  ~player();
  player(std::string);
  player();
  uint32_t id;
  std::string nickname;
  int   *get_input();
  int   type;
  virtual int get_ping();
  actor *character;
  void init(actor*);
};

class network_p : public player{
 public:
  ~network_p();
  network_p();
  network_p(std::string, sockaddr_storage *addr);
  struct sockaddr_storage *address;
  void      *input_buffer;
  int       input_buffer_size;
  int       get_ping();
};

class local_p : public player{
 public:
  ~local_p();
  local_p();
  int get_ping();
  std::list<command_binding> control_scheme;
  local_p(std::string);
};

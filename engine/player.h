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
  virtual void ping();
  actor *character;
  void init(actor*);
  unsigned int last_ping_time=0;
  unsigned int last_ping=0;
};

class network_p : public player{
 public:
  ~network_p();
  network_p();
  network_p(std::string, sockaddr_storage *addr);
  struct sockaddr_storage *address;
  unsigned int address_length = sizeof(struct sockaddr_in);
  void      *input_buffer;
  int       input_buffer_size;
  void       ping();
};

class local_p : public player{
 public:
  ~local_p();
  local_p();
  std::list<command_binding> control_scheme;
  local_p(std::string);
};

#include "engine.h"

void
player::init(actor* actor)
{
  character = actor;
  actor->controller = this;
  return;
}

player::player()
{
  return;
}

player::player(std::string _nickname)
{
  nickname = _nickname;
  return;
}

player::~player()
{
  return;
}

local_p ::local_p(std::string _nickname)
{
  for (unsigned int i = 0; i < _default_bindings.size(); i++)
    control_scheme.push_back(_default_bindings[i]);
  return;
}

void
player ::ping()
{
  return;
}

void
network_p ::ping()
{
  net_message msg;
  msg.operation = NET_PING;
  msg.frequency = 1;
  msg.attempts = 1;
  msg.data = NULL;
  msg.data_size = 0;
  memcpy(&msg.address, address, sizeof(struct sockaddr_storage));
  net_add_message(&msg, false);
  return;
}

network_p::network_p(std::string str, struct sockaddr_storage* addr)
{
  address = (sockaddr_storage*)malloc(sizeof(sockaddr_storage));
  memcpy(address, addr, sizeof(struct sockaddr));
  nickname = str;
  return;
}

network_p::~network_p()
{
  return;
}

// network_p::network_p(){
// return;
//}

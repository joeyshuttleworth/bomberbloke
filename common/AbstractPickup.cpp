#include "AbstractPickup.hpp"
#include "bloke.hpp"

void
AbstractPickup::update()
{
  if (!_server)
    return;
  std::list<std::shared_ptr<actor>> actor_list =
    _pScene->ActorsCollidingWith(this);
  for (auto i = actor_list.begin(); i != actor_list.end(); i++) {
    if ((*i)->getType() == ACTOR_BLOKE) {
      log_message(DEBUG, "bloke picked up pickup");
      std::shared_ptr<bloke> bloke1 = std::dynamic_pointer_cast<bloke>(*i);
      pickup(bloke1);
      remove();
      if (auto p = bloke1->getPlayer())
        _net_server->syncPlayerProperties(p);
      break;
    }
  }
}

/*
  Event used to update a player's metadata.

  This part of the protocol is suitable for 'slow', intermittent information
  that is visible to all players e.g. ping level.
*/

#ifndef METADATAEVENT_HPP
#define METADATAEVENT_HPP

#include <memory>
#include <engine.hpp>
#include <cereal/types/map.hpp>
#include "AbstractEvent.hpp"
#include "Metadata.hpp"
#include <map>


class MetadataEvent :  public  AbstractEvent{
public:
  // Store delta for each player ('mActorId')
  std::map<int, Metadata> delta;

  int getType() const override{
    return EVENT_METADATA;
  }

  MetadataEvent()= default;

  void includeUpdateNumeric(int actorId, const std::string& key, int &value) {
    delta[actorId].numeric[key] = value;
  }
  void includeUpdateCharacter(int actorId, const std::string& key, std::string &value) {
    delta[actorId].character[key] = value;
  }

  void applyUpdate() {
    if(_server) {
      log_message(ERR, "Server does not accept metadata updates");
      return;
    }

    for(auto const &p : delta) {
      int id = p.first;
      Metadata update = p.second;

      std::shared_ptr<AbstractPlayer> player = findPlayer((unsigned int) id);
      if(player == nullptr)
        log_message(INFO,
                    "Recieved metadata for player id " + std::to_string(id)
                      + " but couldn't find them");
      else {
        update.applyTo(player->mMetadata);
      }
    }
  }

  template<class Archive>
  /*Used by cereal to serialize the event for it to be sent/received*/
  void serialize(Archive &archive){
    archive(cereal::base_class<AbstractEvent>(this), delta);
  }
};

CEREAL_REGISTER_TYPE(MetadataEvent)

#endif

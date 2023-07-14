//
// Created by dave on 08.06.20.
//

#include "NetClient.hpp"
#include "AbstractEvent.hpp"
#include "CommandEvent.hpp"
#include "CreationEvent.hpp"
#include "PlayerLeaveEvent.hpp"
#include "JoinEvent.hpp"
#include "KickEvent.hpp"
#include "MetadataEvent.hpp"
#include "MoveEvent.hpp"
#include "PlayerPropertiesEvent.hpp"
#include "QueryEvent.hpp"
#include "RemovalEvent.hpp"
#include "ServerInfoEvent.hpp"
#include "SyncEvent.hpp"
#include "acceptEvent.hpp"
#include "actor.hpp"
#include "engine.hpp"
#include "syncEvent.hpp"
#include "errorEvent.hpp"
#include <cereal/archives/portable_binary.hpp>
#include <iostream>
#include <memory>
#include <string>

NetClient::NetClient()
{}

NetClient::~NetClient()
{}

bool
NetClient::connectClient(std::string serverAddress, ushort port)
{
  mServerId = mConnector->connectPeer(serverAddress, port);
  return mServerId > -1;
}

bool
NetClient::joinBlokeServer(std::string address, const std::string& nickname, const std::vector<std::string>& commands)
{
  mConnector.reset(new ENetConnector());
  mConnector->open();

  ushort port;

  /*  Attempt to parse the first argument as address:port.
      If no ':' is present, the port number defaults to 8888.
  */
  long unsigned int delim_pos = address.find(':');
  if (delim_pos == std::string::npos) {
    port = 8888;
  } else if (address.substr(delim_pos + 1).find(':') != std::string::npos) {
    std::stringstream msg;
    msg << "Couldn't parse address, " << address;
    log_message(ERR, msg.str());
    return false;
  } else {
    address = address.substr(0, delim_pos);
    /*  TODO replace try-catch with something less lazy to check if
        we're going to have an error
    */
    try {
      port = (ushort) std::stoi(address.substr(delim_pos + 1));
    } catch (std::exception& e) {
      std::stringstream msg;
      msg << "Failed to parse port number \n"
          << e.what() << "defaulting to 8888";
      log_message(ERR, msg.str());
      port = 8888;
    }
  }

  if (!connectClient(address, port))
    return false;

  /* Query the server to see if we're able to join */
  std::unique_ptr<AbstractEvent> q_event(new QueryEvent(nickname));
  mConnector->sendEvent(std::move(q_event), mServerId);

  /* We expect ServerInfoEvent */
  std::set<EventType> outcomes = { EVENT_INFO };
  auto response = mConnector->pollFor(5000, outcomes);
  if(response.second == nullptr) {
    return false;
  }
  std::shared_ptr<ServerInfoEvent> info_event =
    std::dynamic_pointer_cast<ServerInfoEvent>(response.second);
  info_event->output();

  /* Check we're allowed our nickname */
  if (!info_event->mUserNameFree) {
    log_message(INFO, "User name not allowed!");
    return false;
  }

  /* Check that there's room for us */
  if (info_event->mMaxPlayers <= info_event->mNumberOfPlayers) {
    log_message(INFO, "Server is full!");
    return false;
  }

  /* Now 'join' and wait at least 10 seconds */
  std::unique_ptr<AbstractEvent> join_event(
    new JoinEvent(nickname, info_event->mMagicNumber, commands));
  mConnector->sendEvent(std::move(join_event), mServerId);
  std::set<EventType> outcomes_2 = { EVENT_ERROR, EVENT_ACCEPT };
  auto joinResponse = mConnector->pollFor(10000, outcomes_2);
  if(joinResponse.first == -1) {
    log_message(ERR, "Timed out");
    return false;
  }
  auto receive_event = joinResponse.second;
  switch (receive_event->getType()) {
    case EVENT_ERROR: {
      /* Handle rejection */
      log_message(INFO,
                  "failed to join server reason \"" + receive_event->output() +
                    "\"");
      return false;
    }
    case EVENT_ACCEPT: {
      /* Handle join  */
      log_message(INFO,
                  "Successfully joined server. Message: " +
                    receive_event->output());
      return true;
    }
    default:
      break;
  }

  log_message(ERR, "Unexpected first response to join attempt");
  return false;
}

void
NetClient::handleServerCommand(std::string str)
{
  _pScene->handleCommand(str);
  return;
}

void
NetClient::pollServer()
{
  if (!isConnected())
    return;

  auto events = mConnector->poll(0);
  for(const auto& event_received : events) {
    // Assumed to be the server in all cases
    std::shared_ptr<AbstractEvent> event = event_received.second;

    switch (event->getType()) {
      case EVENT_SYNC: {
        std::shared_ptr<SyncEvent> s_event =
          std::dynamic_pointer_cast<SyncEvent>(event);
        mPlayers = s_event->getPlayers();
        _pScene->mState = s_event->mState;
        /* TODO move mPlayers to _player_list */
        auto iter =
          std::find_if(mPlayers.begin(),
                       mPlayers.end(),
                       [](serverPlayer sp) -> bool { return sp.isLocal(); });
        if (iter != mPlayers.end()) {
          if (iter->mNickname != _nickname) {
            log_message(
              ERR,
              "sync player list had wrong nickname for the local player");
            break;
          }
          /*  Reset player list */
          _player_list = {};
          if (_local_player_list.size() == 0)
            _local_player_list.push_back(
              LocalPlayer(iter->getNickname(), iter->getId()));
          _local_player_list.back().setId(iter->getId());
          int player_id = iter->getId();
          for (auto i = _pScene->mActors.begin(); i != _pScene->mActors.end();
               i++) {
            if (!*i)
              continue;
            if ((*i)->getPlayerId() == player_id)
              _pScene->linkActorToPlayer((*i), player_id);
          }
        }
        _pScene->init();
        auto p_list = s_event->getPlayers();
        for (auto i = p_list.begin(); i != p_list.end(); i++) {
          std::shared_ptr<AbstractPlayer> p =
            std::make_shared<serverPlayer>(*i);
          _player_list.push_back(p);
        }
        log_message(DEBUG, "synced with server");
        break;
      }
      case EVENT_COMMAND: {
        std::shared_ptr<CommandEvent> c_event =
          std::dynamic_pointer_cast<CommandEvent>(event);
        handleServerCommand(c_event->getCommand());
        break;
      }
      case EVENT_MOVE: {
        std::shared_ptr<MoveEvent> m_event =
          std::dynamic_pointer_cast<MoveEvent>(event);
        std::shared_ptr<actor> p_actor = _pScene->GetActor(m_event->mActorId);
      /* Make the pointer shared so we can handle it elsewhere */
      std::shared_ptr<AbstractEvent> sp_to_handle = std::move(receive_event);
      switch (sp_to_handle->getType()) {
        case EVENT_SYNC: {
          std::shared_ptr<SyncEvent> s_event =
            std::dynamic_pointer_cast<SyncEvent>(sp_to_handle);
          mPlayers = s_event->getPlayers();
          _pScene->mState = s_event->mState;
          /* TODO move mPlayers to _player_list */
          auto iter =
            std::find_if(mPlayers.begin(),
                         mPlayers.end(),
                         [](serverPlayer sp) -> bool { return sp.isLocal(); });
          if (iter != mPlayers.end()) {
            if (iter->mNickname != _nickname) {
              log_message(
                ERR,
                "sync player list had wrong nickname for the local player");
              break;
            }
            /*  Reset player list */
            _player_list = {};
            if (_local_player_list.size() == 0)
              _local_player_list.push_back(
                LocalPlayer(iter->getNickname(), iter->getId()));
            _local_player_list.back().setId(iter->getId());
            unsigned int player_id = iter->getId();
            for (auto i = _pScene->mActors.begin(); i != _pScene->mActors.end();
                 i++) {
              if (!*i)
                continue;
              if ((*i)->getPlayerId() == player_id)
                _pScene->linkActorToPlayer((*i), player_id);
            }
          }
          _pScene->init();
          auto p_list = s_event->getPlayers();
          for (auto i = p_list.begin(); i != p_list.end(); i++) {
            std::shared_ptr<AbstractPlayer> p =
              std::make_shared<serverPlayer>(*i);
            _player_list.push_back(p);
          }
          log_message(DEBUG, "synced with server");
          break;
        }

        case EVENT_METADATA: {
          std::shared_ptr<MetadataEvent> m_event =
            std::dynamic_pointer_cast<MetadataEvent>(sp_to_handle);
          m_event->applyUpdate();
          break;
        }

        case EVENT_COMMAND: {
          std::shared_ptr<CommandEvent> c_event =
            std::dynamic_pointer_cast<CommandEvent>(sp_to_handle);
          handleServerCommand(c_event->getCommand());
          break;
        }

        case EVENT_MOVE: {
          std::shared_ptr<MoveEvent> m_event =
            std::dynamic_pointer_cast<MoveEvent>(sp_to_handle);
          std::shared_ptr<actor> p_actor = _pScene->GetActor(m_event->mActorId);

        if (!p_actor) {
          log_message(ERR,
                      "Error couldn't find actor with id " +
                        std::to_string(m_event->mActorId));
          break;
        } else {
          dvector position = { m_event->mPosition[0], m_event->mPosition[1] };
          dvector velocity = { m_event->mVelocity[0], m_event->mVelocity[1] };
          p_actor->addState(position, velocity, m_event->mTick);
        }
        break;
      }
      case EVENT_CREATE: {
        std::shared_ptr<CreationEvent> c_event =
          std::dynamic_pointer_cast<CreationEvent>(event);
        if (c_event->getActor())
          _pScene->addActorWithId(c_event->getActor());
        else if (c_event->getParticle())
          _pScene->mParticleList.push_back(c_event->getParticle());
        else {
          log_message(ERR, "Received malformed create event");
        }
        break;
      }
      case EVENT_REMOVE: {
        std::shared_ptr<RemovalEvent> r_event =
          std::dynamic_pointer_cast<RemovalEvent>(event);
        std::shared_ptr<actor> a = _pScene->GetActor(r_event->getId());
        if (!a)
          log_message(ERR, "Couldn't find requested actor to remove");
        else
          a->remove();
        break;
      }
      case EVENT_PROPERTIES: {
        std::shared_ptr<PlayerPropertiesEvent> p_event =
          std::dynamic_pointer_cast<PlayerPropertiesEvent>(event);
        /* TODO handle local multiple players */
        GamePlayerProperties props = p_event->getProperties();
        std::shared_ptr<AbstractPlayerProperties> p_properties =
          std::make_shared<GamePlayerProperties>(props);
        _local_player_list.back().resetPlayerProperties(p_properties);
        break;
      }
      case EVENT_KICK: {
        std::shared_ptr<KickEvent> p_event =
          std::dynamic_pointer_cast<KickEvent>(event);
        log_message(INFO,
                    "Kicked by server. Reason: " + p_event->getReason());
        disconnectClient();
      }
      default:
        break;
    }
  }
  return;
}

void
NetClient::disconnectClient()
{
  std::unique_ptr<AbstractEvent> leave_event(new PlayerLeaveEvent());
  mConnector->sendEvent(std::move(leave_event), mServerId);
  mConnector->close();
}

bool
NetClient::isConnected()
{
  return mConnector != nullptr && mConnector->countPeers() == 1;
}

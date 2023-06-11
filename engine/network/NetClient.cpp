//
// Created by dave on 08.06.20.
//

#include "NetClient.hpp"
#include "AbstractEvent.hpp"
#include "CommandEvent.hpp"
#include "CreationEvent.hpp"
#include "JoinEvent.hpp"
#include "KickEvent.hpp"
#include "MoveEvent.hpp"
#include "PlayerPropertiesEvent.hpp"
#include "QueryEvent.hpp"
#include "RemovalEvent.hpp"
#include "ServerInfoEvent.hpp"
#include "acceptEvent.hpp"
#include "actor.hpp"
#include "engine.hpp"
#include "errorEvent.hpp"
#include "syncEvent.hpp"
#include <cereal/archives/portable_binary.hpp>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

NetClient::NetClient()
{
  if (enet_initialize() != 0) {
    printf("ERROR: An error occurred while initializing ENet");
    return;
  }
  // TODO: insert variables for host
  mENetHost = enet_host_create(nullptr, 1, 1, 0, 0);
}

NetClient::~NetClient()
{
  // Clean up ENet
  enet_host_destroy(mENetHost);
  enet_deinitialize();
}

bool
NetClient::connectClient(std::string serverAddress, enet_uint16 port)
{

  ENetEvent event;

  mServerAddress = serverAddress;
  mPort = port;
  log_message(INFO, "attempting to connect");
  enet_address_set_host(&mENetServerAddress, mServerAddress.c_str());
  mENetServerAddress.port = mPort;

  // Initiate the connection, with only one channel
  if (mENetHost)
    enet_host_destroy(mENetHost);
  mENetHost = enet_host_create(nullptr, 1, 1, 0, 0);
  enet_host_compress_with_range_coder(mENetHost);
  mENetServer = enet_host_connect(mENetHost, &mENetServerAddress, 1, 0);
  if (mENetServer == NULL) {
    fprintf(stderr, "No available peers for initiating an ENet connection.\n");
    return false;
  }
  // Wait up to 5 seconds for the connection attempt to succeed. */
  if (enet_host_service(mENetHost, &event, 5000) > 0 &&
      event.type == ENET_EVENT_TYPE_CONNECT) {
    std::stringstream msg;
    msg << "Connection to " << serverAddress << ":" << port << " succeeded";
    // log_message(INFO, msg.str());
    return true;

  } else {
    enet_peer_reset(mENetServer);
    std::stringstream msg;
    msg << "Connection to " << mServerAddress << ":" << port << " failed";
    // log_message(INFO, msg.str());
    return false;
  }
}

bool
NetClient::joinBlokeServer(std::string address, const std::string& nickname, const std::vector<std::string>& commands)
{
  ENetEvent event;
  int port;

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
      port = std::stoi(address.substr(delim_pos + 1));
    } catch (std::exception& e) {
      std::stringstream msg;
      msg << "Failed to parse port number \n"
          << e.what() << "defaulting to 8888";
      log_message(ERR, msg.str());
      port = 8888;
    }
  }

  if (connectClient(address, port) == false) {
    return false;
  }


  /* Query the server to see if we're able to join */
  std::unique_ptr<AbstractEvent> q_event(new QueryEvent(nickname));
  sendEvent(q_event);

  while (enet_host_service(mENetHost, &event, 5000) > 0) {
    // event occured
    std::unique_ptr<AbstractEvent> receive_event;
    std::stringstream data_in;
    data_in.write((char*)event.packet->data, event.packet->dataLength);

    try {
      cereal::PortableBinaryInputArchive inArchive(data_in);
      inArchive(receive_event);
      if (receive_event->getType() != EVENT_INFO) {
        /* Make the pointer shared so we can handle it elsewhere */
        std::shared_ptr<AbstractEvent> sp_to_handle = std::move(receive_event);
      }
    } catch (std::exception& e) {
      std::stringstream strstream;
      strstream << "Received malformed network event.\n" << e.what();
      log_message(ERR, strstream.str());
    }

    /*The event is a ServerInfoEvent*/
    std::shared_ptr<AbstractEvent> tmp_event(std::move(receive_event));
    std::shared_ptr<ServerInfoEvent> info_event =
      std::dynamic_pointer_cast<ServerInfoEvent>(tmp_event);
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

    std::unique_ptr<AbstractEvent> join_event(
                                          new JoinEvent(nickname, info_event->mMagicNumber, commands));
    sendEvent(join_event);
    /*  now poll the server again (10 seconds)*/
    while (enet_host_service(mENetHost, &event, 10000) > 0) {
      std::unique_ptr<AbstractEvent> receive_event;
      std::stringstream data_in;
      data_in.write((char*)event.packet->data, event.packet->dataLength);
      {
        cereal::PortableBinaryInputArchive inArchive(data_in);
        inArchive(receive_event);
        log_message(INFO, "data waiting for join " + data_in.str());
      }
      switch (receive_event->getType()) {
        case EVENT_ERROR: {
          /* Handle rejection */
          log_message(INFO,
                      "failed to join server reason \"" +
                        receive_event->output() + "\"");
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
    }
  }
  log_message(ERR, "timed out");
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
  ENetEvent event;
  while (enet_host_service(mENetHost, &event, 0) > 0) {
    if (event.type == ENET_EVENT_TYPE_RECEIVE) {
      // event occured
      std::stringstream data_in;
      data_in.write((char*)event.packet->data, event.packet->dataLength);
      std::unique_ptr<AbstractEvent> receive_event;
      try {
        cereal::PortableBinaryInputArchive inArchive(data_in);
        inArchive(receive_event);
      } catch (std::exception& ex) {
        std::stringstream strstream;
        strstream << "Received malformed network event\n" << ex.what();
        log_message(ERR, strstream.str());
        break;
      }

      /* Make the pointer shared so we can handle it elsewhere */
      std::shared_ptr<AbstractEvent> sp_to_handle = std::move(receive_event);
      switch (sp_to_handle->getType()) {
        case EVENT_SYNC: {
          std::shared_ptr<syncEvent> s_event =
            std::dynamic_pointer_cast<syncEvent>(sp_to_handle);
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
            std::dynamic_pointer_cast<CreationEvent>(sp_to_handle);
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
            std::dynamic_pointer_cast<RemovalEvent>(sp_to_handle);
          std::shared_ptr<actor> a = _pScene->GetActor(r_event->getId());
          if (!a)
            log_message(ERR, "Couldn't find requested actor to remove");
          else
            a->remove();
          break;
        }
        case EVENT_PROPERTIES: {
          std::shared_ptr<PlayerPropertiesEvent> p_event =
            std::dynamic_pointer_cast<PlayerPropertiesEvent>(sp_to_handle);
          /* TODO handle local multiple players */
          GamePlayerProperties props = p_event->getProperties();
          std::shared_ptr<AbstractPlayerProperties> p_properties =
            std::make_shared<GamePlayerProperties>(props);
          _local_player_list.back().resetPlayerProperties(p_properties);
          break;
        }
        case EVENT_KICK: {
          std::shared_ptr<KickEvent> p_event =
            std::dynamic_pointer_cast<KickEvent>(sp_to_handle);
          log_message(INFO,
                      "Kicked by server. Reason: " + p_event->getReason());
          disconnectClient();
        }
        default:
          break;
      }
    }
  }
  return;
}

void
NetClient::disconnectClient()
{
  if (!mENetServer || !mENetHost)
    return;
  // An ENET_EVENT_DISCONNECT event will be generated by enet_host_service()
  // once the disconnection is complete.
  // We need to poll, as we could recieve other packets first?
  ENetEvent event;
  enet_peer_disconnect(mENetServer, 0);
  while (enet_host_service(mENetHost, &event, 3000) > 0) {
    switch (event.type) {
      case ENET_EVENT_TYPE_DISCONNECT:
        printf("Disconnection succeeded.\n");
        return;
      default:
        enet_packet_destroy(event.packet);
        break;
    }
  }
  // Disconnect didn't happen in three seconds, force reset peer
  enet_peer_reset(mENetServer);
  log_message(INFO, "Forced disconnection from server");
}

void
NetClient::sendStringMessage(std::string message)
{
  ENetPacket* packet = enet_packet_create(
    message.c_str(), strlen(message.c_str()) + 1, ENET_PACKET_FLAG_RELIABLE);
  enet_peer_send(mENetServer, 0, packet);
  enet_host_flush(mENetHost);
  log_message(DEBUG, "Sent Message" + message);
}

void
NetClient::sendEvent(std::unique_ptr<AbstractEvent>& event)
{
  std::stringstream blob;
  bool reliable;
  switch (event->getType()) {
    case EVENT_QUERY:
    case EVENT_JOIN:
    case EVENT_COMMAND:
      reliable = true;
      break;
    default:
      reliable = false;
      break;
  }

  try {
    cereal::PortableBinaryOutputArchive oArchive(blob);
    oArchive(event);
  } catch (std::exception&) {
    log_message(ERR, "Failed to send event");
    return;
  }

  ENetPacket* packet;
  std::string message = blob.str();

  reliable = true;

  if (reliable)
    packet = enet_packet_create(
    message.c_str(), message.size(), ENET_PACKET_FLAG_RELIABLE);
  else
    packet = enet_packet_create(message.c_str(), message.size(), 0);

  enet_peer_send(mENetServer, 0, packet);
  return;
}

bool
NetClient::isConnected()
{
  if (mENetHost != nullptr) {
    return true;
  }
  return false;
}

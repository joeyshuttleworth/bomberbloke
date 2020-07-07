//
// Created by dave on 08.06.20.
//

#include "engine.hpp"
#include "CreateEvent.hpp"
#include "RemoveEvent.hpp"
#include "NetClient.hpp"
#include "CommandEvent.hpp"
#include "ServerInfoEvent.hpp"
#include "JoinEvent.hpp"
#include "acceptEvent.hpp"
#include  "syncEvent.hpp"
#include "errorEvent.hpp"
#include "MoveEvent.hpp"
#include "actor.hpp"
#include <string>
#include <iostream>
#include <sstream>
#include "AbstractEvent.hpp"
#include "QueryEvent.hpp"
#include <memory>


NetClient::NetClient(){
    if (enet_initialize() != 0) {
        printf("ERROR: An error occurred while initializing ENet");
        return;
    }
    //TODO: insert variables for host
    mENetHost = enet_host_create(nullptr, 1, 1, 0, 0);
}

NetClient::~NetClient() {
    // Clean up ENet
    enet_host_destroy(mENetHost);
    enet_deinitialize();
}

bool NetClient::connectClient(std::string serverAddress, enet_uint16 port) {

    ENetEvent event;

    mServerAddress = serverAddress;
    mPort = port;
    log_message(INFO, "attempting to connect");
    enet_address_set_host(&mENetServerAddress, mServerAddress.c_str());
    mENetServerAddress.port = mPort;

    //Initiate the connection, with only one channel
    if(mENetHost)
      enet_host_destroy(mENetHost);
    mENetHost = enet_host_create(nullptr, 1, 1, 0, 0);
    mENetServer = enet_host_connect(mENetHost, &mENetServerAddress, 1, 0);
    if (mENetServer == NULL) {
        fprintf(stderr, "No available peers for initiating an ENet connection.\n");
        return false;
    }
    // Wait up to 5 seconds for the connection attempt to succeed. */
    if (enet_host_service(mENetHost, &event, 5000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT) {
        std::stringstream msg;
        msg << "Connection to " << serverAddress << ":" <<  port << " succeeded";
        log_message(INFO, msg.str());
        return true;

    } else {
        enet_peer_reset(mENetServer);
        std::stringstream msg;
        msg << "Connection to " << mServerAddress << ":" <<  port << " failed";
        log_message(INFO, msg.str());
        return false;
    }
}

bool NetClient::joinBlokeServer(std::string address, int port, std::string nickname){
  ENetEvent event;
  if(connectClient(address, port) == false){
    return false;
  }

  /* Query the server to see if we're able to join */
  std::unique_ptr<AbstractEvent> q_event(new QueryEvent(nickname));
  sendEvent(q_event);

  while(enet_host_service(mENetHost, &event, 5000) > 0){
    // event occured
    std::unique_ptr<AbstractEvent> receive_event;
    std::stringstream data_in;
    data_in << event.packet->data;
    {cereal::JSONInputArchive inArchive(data_in);
    // cereal::JSONInputArchive inArchive(data_in);
    log_message(DEBUG, "received message: " + data_in.str());
    inArchive(receive_event);
    }
    if(receive_event->getType() != EVENT_INFO){
    /* Make the pointer shared so we can handle it elsewhere */
    std::shared_ptr<AbstractEvent> sp_to_handle = std::move(receive_event);
    }
    else{
      /*The event is a ServerInfoEvent*/
      std::shared_ptr<AbstractEvent> tmp_event(std::move(receive_event));
      std::shared_ptr<ServerInfoEvent> info_event = std::dynamic_pointer_cast<ServerInfoEvent>(tmp_event);
      info_event->output();

      /* Check we're allowed our nickname */
      if(!info_event->mUserNameFree){
        log_message(INFO, "User name not allowed!");
        return false;
      }

      /* Check that there's room for us */

      if(info_event->mMaxPlayers <= info_event->mNumberOfPlayers){
        log_message(INFO, "Server is full!");
        return false;
      }

      std::unique_ptr<AbstractEvent> join_event(new JoinEvent(nickname, info_event->mMagicNumber));

      sendEvent(join_event);

      /*  now poll the server again (10 seconds)*/
      while(enet_host_service(mENetHost, &event, 10000) > 0){
        std::unique_ptr<AbstractEvent> receive_event;
        std::stringstream data_in;
        data_in << event.packet->data;

        {
          cereal::JSONInputArchive inArchive(data_in);
          log_message(DEBUG, "received message: " + data_in.str());
          inArchive(receive_event);
        }
        switch(receive_event->getType()){
        case EVENT_ERROR:{
            /* Handle rejection */
            log_message(INFO, "failed to join server reason \"" + receive_event->output() + "\"");
            return false;
          }
        case EVENT_ACCEPT:{
            /* Handle join  */
            log_message(INFO, "Successfully joined server. Message: " + receive_event->output());
            return true;
          }
        default:break;
        }
      }
    }
  }
  return false;
}

void NetClient::pollServer(){
  if(!isConnected())
    return;
  ENetEvent event;
  while(enet_host_service(mENetHost, &event, 0)>0){
   if (event.type == ENET_EVENT_TYPE_RECEIVE) {
     // event occured
     std::stringstream data_in;
     data_in << event.packet->data;
     std::unique_ptr<AbstractEvent> receive_event;
     try{
       cereal::JSONInputArchive inArchive(data_in);
       inArchive(receive_event);
     }
     catch(std::exception &ex){
       log_message(ERROR, "Tried parsing " + data_in.str());
       std::cout << ex.what() << "\n";
     }

     /* Make the pointer shared so we can handle it elsewhere */
     std::shared_ptr<AbstractEvent> sp_to_handle = std::move(receive_event);
     switch(sp_to_handle->getType()){
     case EVENT_SYNC:{
       std::shared_ptr<syncEvent> s_event = std::dynamic_pointer_cast<syncEvent>(sp_to_handle);
       mPlayers = s_event->getPlayers();
       auto iter = std::find_if(mPlayers.begin(), mPlayers.end(), [](serverPlayer sp) -> bool{return sp.isLocal();});
       if(iter != mPlayers.end()){
         if(iter->mNickname != _nickname){
           log_message(ERROR, "sync player list had wrong nickname for the local player");
           break;
         }
         if(_local_player_list.size()==0)
           _local_player_list.push_back(_nickname);
         _local_player_list.back().setId(iter->getId());
         unsigned int player_id = _local_player_list.back().getId();
         for(auto i = _pScene->mActors.begin(); i != _pScene->mActors.end(); i++){
           if((*i)->getPlayerId() == player_id)
             _local_player_list.back().setCharacter(*i);
         }
       }
       log_message(DEBUG, "synced with server");
       break;
        }
     case EVENT_MOVE:{
       std::shared_ptr<MoveEvent> m_event = std::dynamic_pointer_cast<MoveEvent>(sp_to_handle);
       std::shared_ptr<actor> p_actor = _pScene->GetActor(m_event->mActorId);

       if(!p_actor){
         log_message(ERROR, "Error couldn't find actor with id " + std::to_string(m_event->mActorId));
         break;
       }
       else{
         dvector position = {m_event->mPosition[0], m_event->mPosition[1]};
         dvector velocity = {m_event->mVelocity[0], m_event->mVelocity[1]};
         p_actor->addState(position, velocity, m_event->mTick);
       }
       break;
     }

     case EVENT_CREATE:{
       std::shared_ptr<CreateEvent> c_event = std::dynamic_pointer_cast<CreateEvent>(sp_to_handle);
       _pScene->addActorWithId(c_event->getActor());
       break;
     }
     case EVENT_REMOVE:{
       std::shared_ptr<RemoveEvent> r_event = std::dynamic_pointer_cast<RemoveEvent>(sp_to_handle);
       std::shared_ptr<actor> a = _pScene->GetActor(r_event->getId());
       if(!a)
         log_message(ERROR, "Couldn't find requested actor to remove");
       else
         a->toRemove();
     }
     default:
       break;
     }
   }
  }
  return;
}



void NetClient::disconnectClient() {
    //An ENET_EVENT_DISCONNECT event will be generated by enet_host_service() once the disconnection is complete.
    // We need to poll, as we could recieve other packets first?
  if(!mENetServer || !mENetHost){
    return;
  }
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
  //Disconnect didn't happen in three seconds, force reset peer
  enet_peer_reset(mENetServer);
}

void NetClient::sendStringMessage(std::string message) {
    ENetPacket *packet = enet_packet_create(message.c_str(), strlen(message.c_str())+1, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(mENetServer, 0, packet);
    enet_host_flush(mENetHost);
    log_message(DEBUG,  "Sent Message" + message);
}

void NetClient::sendEvent(std::unique_ptr<AbstractEvent> &event){
  std::stringstream blob;
  {
    cereal::JSONOutputArchive oArchive(blob);
    oArchive(event);
  }
  sendStringMessage(blob.str());

  log_message(DEBUG, "Sending" + blob.str());
  return;
}


bool NetClient::isConnected() {
    if (mENetHost != nullptr) {
        return true;
    }
    return false;
}

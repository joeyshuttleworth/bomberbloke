#include "engine.h"
#include "server.h"

bool _server = true;
bool _draw   = true;
unsigned int _ping_time = 0;
const std::vector<command_binding> _default_bindings;

void add_client(std::string nickname, struct sockaddr_storage *addr){
  network_p client = network_p(std::string(nickname), addr);
  client.last_ping_time = _tick;
  _client_list.push_back(client);
}

std::list<network_p>::iterator get_client(sockaddr_storage *addr){
  for(auto client = _client_list.begin(); client!=_client_list.end(); client++){
    int family = addr->ss_family;
    if(family == client->address->ss_family){
      if(family == AF_INET){
	if(((sockaddr_in*)addr)->sin_addr.s_addr == ((sockaddr_in*)client->address)->sin_addr.s_addr && ((sockaddr_in*)addr)->sin_port == ((sockaddr_in*)client->address)->sin_port)
	  return client;
	
      }
      else if(family == AF_INET6){
	//TODO IPV6
      }
    }
  }
return _client_list.end();
}

void remove_client(std::list<network_p>::iterator player){
  std::string msg = "Removed player " + player->nickname + "\n";
  net_remove_messages_to(player->address);
  if(!_client_list.empty())
    _client_list.erase(player);
  log_message(INFO, msg.c_str());
 return;
}

void server_loop(){
  unsigned int last_tick, current_tick=0;
  int delay;
  timespec t1, t2;
  t2.tv_nsec = 0;
  t2.tv_sec  = 0;
  while(true){
    printf("%d\n", _tick);
    t1 = t2;
    do{
      sleep(0);
      if(clock_gettime(CLOCK_REALTIME, &t2)==-1)
	 log_message(ERROR, "Failed to get time");
    }while(t2.tv_nsec - t1.tv_nsec + (float)(BILLION * (t2.tv_sec - t1.tv_sec))  < (float)BILLION/(float)TICK_RATE);
    if(_client_list.empty()){
      _state = DISCONNECTED;
    }else{
      if(_tick % NET_RATE == 0){
	net_flush_messages();
	if(_tick % (5 * TICK_RATE) == 0){
	  _ping_time = _tick; 
	  for(auto i = _client_list.begin(); i != _client_list.end(); i++){
	    /*TODO: Remove all ping messages from queue*/
	    if(_tick - i->last_ping_time > 30*TICK_RATE){
	      remove_client(i);
	      if(_client_list.empty())
		break;
	    }
	    else if(_tick < i->last_ping)
	      i->last_ping = 0;
	    else i->ping();
	  }
	}
      }
    }
    if(_state == PLAYING){
      for(auto i=_level.actor_list.begin(); i!=_level.actor_list.end(); i++){
	  auto prev=i;
	  i++;
	  prev->update();
      }
      _level.actor_list.remove_if([](actor a){return a.remove;});
    }
    if(_state == PLAYING)
      handle_movement();
    draw_screen();
    _tick++;
  }
  return;
}

void handle_datagram(char *buf, struct sockaddr_storage *client_addr, unsigned int count, unsigned int addr_len){
  char opcode = buf[0];
  std::cout  << "Received datagram: " << buf[0] << buf << "\n";
  switch(opcode){
  case NET_MSG:{
    net_message msg;
    std::string str;
    str = get_client(client_addr)->nickname + ": " + std::string(buf+2);
    msg.data = (char*)malloc(str.length());
    msg.data_size = str.length();
    strncpy(msg.data, str.c_str(), str.length());
    msg.operation = NET_MSG;
    msg.attempts = 10;
    msg.frequency= 10 * TICK_RATE/NET_RATE;
    for(auto i = _client_list.begin(); i != _client_list.end(); i++){
      memcpy(&msg.address, i->address, sizeof(struct sockaddr_in));
      msg.address_length = sizeof(struct sockaddr_in);
      net_add_message(&msg);
    }
    free(msg.data);
    break;
  }
  case NET_JOIN:{
    char nickname[count];
    memcpy(&nickname, buf + 2, count - 1);
    buf[count] = '\0';
    nickname[count-1] = '\0';
    printf("JOIN REQUEST RECEIVED FROM %s\n", nickname);
    if(get_client(client_addr)!=_client_list.end()){
      net_message msg;
      msg.operation = NET_ACK;
      msg.data = (char*)malloc(sizeof(char));
      msg.data[0]= buf[1];
      msg.data_size = strlen(msg.data);
      msg.address_length = addr_len;
      msg.frequency = 0;
      msg.attempts  = 1;
      memcpy(&msg.address,client_addr, addr_len); 
      net_add_message(&msg);
      log_message(DEBUG, "client is already joined\n");
    }
    else{
      net_message msg;
      msg.operation = NET_ACK;
      msg.data = (char *)malloc(sizeof(char));
      msg.data[0]  = buf[1];
      msg.data_size = 1;
      msg.address_length = addr_len;
      msg.frequency = 0;
      msg.attempts  = 1;
      memcpy(&msg.address, client_addr, addr_len); 
      net_add_message(&msg);
      add_client(nickname, client_addr);
      std::cout << nickname << " has joined the server" << "\n";
      if(_state == DISCONNECTED)
	_state = PAUSED;
    }
    break;
  }
  case NET_LEAVE:{
    auto client = get_client(client_addr);
    if(client!=_client_list.end())
      remove_client(client);
    else
      log_message(DEBUG, "Leave request received from non-connected player\n");
    break;
  }
  case NET_MOVE:{
    
    break;
  }
  case NET_ACK:{
    unsigned int message_id = buf[1];
    net_message_node *current = net_get_message(message_id);
    bool malformed = false;
    pthread_mutex_lock(&net_out_mutex);
    net_remove_message(current);
    pthread_mutex_unlock(&net_out_mutex);
    if(!current)
      malformed = true;
    else if(!current->msg)
      malformed = true;
    if(malformed){
      log_message(DEBUG, "MALFORMED NET_ACK\n");
      return;
    }
    if(count < 2)
      return;
    switch(current->msg->operation){
    default:
      break;
    case NET_PING:{
      std::cout << "Received PONG\n";
      std::list<network_p>::iterator client = get_client(client_addr);
      client->last_ping_time = _tick;
      client->last_ping      = _ping_time - _tick;
    }
    case NET_NEW_GAME:
      network_p client = *get_client(client_addr);
      client.state = PAUSED;
      break;
    }
  }
  default:
    break;
  }
  return;
}

void timeout(net_message_node *node){
  net_message *msg = node->msg;
  switch(msg->operation){
  default:
    break;
  }
  return;
}

void send_player_list(){
  char *player_list;

  for(auto i = _client_list.begin();  i!= _client_list.end(); i++){

  }
  return;
}

void sync_players(){
  char buf[513];
  int c, j = 1;
  net_message msg;
  //todo add _client_list mutex and actor_list mutex
  c = _client_list.size();
  if(c > 255){
    log_message(ERROR, "Client list is far too large\n");
    return;    
  }
  buf[0] = c;
  c = 1;
  for(auto i = _client_list.begin(); i!= _client_list.end(); i++){
    int len = i->nickname.length();
    if(len == 0){
      log_message(ERROR, "Empty nickname\n");
      return;
    }
    strncpy(buf, i->nickname.c_str(), len);
    c = c + len;
    i->id = j;
    j++;
  }
  j = 0;
  for(auto i = _level.actor_list.begin(); i != _level.actor_list.end(); i++){
    net_float tmp;
    i->id = j;
    j++;
    if(i->controller == NULL)
      buf[c] = 0;
    else{
      buf[c] = i->controller->id;
    }
    if(c + 4*sizeof(net_float) > 511){
      log_message(ERROR, "NET_SYNC Package too large");
      return;
    }
    buf[c] = i->dim[0];
    buf[c+1] = i->dim[1];
    c = c + 2 * sizeof(char);
    net_float_create(i->position[0], &tmp);
    memcpy(buf + c, &tmp, sizeof(net_float));
    c = c + sizeof(net_float);
    net_float_create(i->position[1], &tmp);
    memcpy(buf + c, &tmp, sizeof(net_float));
    c = c + sizeof(net_float);
    net_float_create(i->velocity[0], &tmp);
    memcpy(buf + c, &tmp, sizeof(net_float));
    c = c + sizeof(net_float);
    net_float_create(i->position[1], &tmp);
    memcpy(buf + c, &tmp, sizeof(net_float));
    c = c + sizeof(net_float);
  }
  msg.data = buf;
  msg.data_size = c;
  msg.frequency = 1;
  msg.attempts  = MAX_ATTEMPTS;
  buf[c+1] = 0;
  log_message(DEBUG, buf);
  send_to_all(&msg);
  return;
}

void engine_new_game(std::string tokens){
  char *actor_list;
  net_message msg;
  log_message(INFO, (char*)"New game ready to start...\n");
  _state = PAUSED;
  new_game(tokens);
  _level = level(10,10);
  if(_server){
    for(auto i = _client_list.begin(); i != _client_list.end(); i++){
      i->state = STOPPED;
    }
  }
  //remove moves and SYNCS from message queue
  sync_players();
  draw_screen();
  return;
}

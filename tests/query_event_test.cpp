#include "NetClient.hpp"
#include "NetServer.hpp"
#include "QueryEvent.hpp"
#include "bloke.hpp"
#include "bomb.hpp"
#include "bomberbloke.h"
#include "cereal/archives/portable_binary.hpp"
#include <SDL2/SDL.h>
#include <assert.h>
#include <thread>

CEREAL_REGISTER_DYNAMIC_INIT(actor)

int
main()
{
  // Don't display any graphics for this test, or even create a window
  _draw = false;
  int rc = 0;
  /* Create a NetClient and NetServer
   * By default the server runs on port 8888
   */
  NetClient net_client;
  NetServer net_server;

  net_server.init(8888);

  std::stringstream data_blob;
  std::stringstream receive_blob;

  std::unique_ptr<AbstractEvent> q_event(new QueryEvent);
  std::unique_ptr<AbstractEvent> test_in_event(new QueryEvent);

  /*  Braces necessary for cereal */
  {
    cereal::PortableBinaryOutputArchive outArchive(data_blob);
    outArchive(q_event);
  }

  cereal::PortableBinaryInputArchive inArchive(data_blob);

  inArchive(test_in_event);

  log_message(DEBUG, "query_event_test event to send is " + data_blob.str());

  /* Wait for a few moments */
  SDL_Delay(1000);
  std::thread server_thread(&NetServer::pollLoop, &net_server);

  if (net_client.connectClient("127.0.0.1", 8888)) {
    net_client.sendEvent(test_in_event);
    SDL_Delay(900);
  }

  else {
    rc = -1;
  }

  _halt = true;
  SDL_Delay(900);
  server_thread.join();

  /* Now it's safe to read from the client */
  net_client.pollServer();

  return rc;
}

void
gameUpdate()
{
  return;
}

void new_game(std::string)
{
  return;
}

#include "NetClient.hpp"
#include "NetServer.hpp"
#include "bloke.hpp"
#include "bomb.hpp"
#include "bomberbloke.h"
#include "cereal/archives/json.hpp"
#include <SDL3/SDL.h>
#include <assert.h>
#include <thread>

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

  assert(net_server.init_enet());

  /* Wait for a few moments */
  SDL_Delay(1000);
  std::thread server_thread(&NetServer::pollLoop, &net_server);

  /* Connect to the server. If uncessful this will exit(EXIT_FAILURE)
   *  so there's no need for additional checking at this stage.
   */
  if (net_client.connectClient("127.0.0.1", 8888)) {
    rc = 0;
    net_client.disconnectClient();
  } else {
    rc = -1;
  }
  _halt = true;
  SDL_Delay(900);
  server_thread.join();
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

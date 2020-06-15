#include "bomberbloke.h"
#include "cereal/archives/json.hpp"
#include <SDL2/SDL.h>
#include "bloke.hpp"
#include "bomb.hpp"
#include "NetClient.hpp"
#include "NetServer.hpp"
#include <thread>


int main() {
    // Don't display any graphics for this test, or even create a window
    _draw = false;

    /* Create a NetClient and NetServer
     * By default the server runs on port 8888
     */
    NetClient net_client;
    NetServer net_server;


    /* Wait for a few moments */
    SDL_Delay(1000);
    std::thread server_thread;

    server_thread = std::thread(&NetServer::poll, &net_server);

    /* Connect to the server. If uncessful this will exit(EXIT_FAILURE)
    *  so there's no need for additional checking at this stage.
    */
    if (net_client.connectClient("127.0.0.1", 8888)) {
        server_thread.detach();
        //stop thread nicely, call destructor
    } else {
        server_thread.detach();
        exit(EXIT_FAILURE);
    }
    
    return 0;
}

void gameUpdate() {
    return;
}

void new_game(std::string) {
    return;
}

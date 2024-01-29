# Bomberbloke [![Build Status](https://travis-ci.com/joeyshuttleworth/bomberbloke.svg?branch=default)](https://travis-ci.com/joeyshuttleworth/bomberbloke)

A multiplayer bomberman clone made in C++.

### Prerequisites

#### Dependencies
You will need the dependencies Cereal, SDL2, ENet and curl.

For Ubuntu 20.04:
```
sudo apt-get install libuv1-dev libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev libenet-dev libsdl2-mixer-dev libcurl4-openssl-dev cmake make build-essential
```
For MacOS:
```
brew install enet sdl2 sdl2_image sdl2_mixer sdl2_ttf
```

#### Building the BLOKE Engine

##### Desktop
The project is setup using CMake:
```
git submodule update --init
cmake -S . -B build && cmake --build build
```

##### Web
When building for web, you will need to do two seperate builds.
The first builds the server (and client) using websockets rather than the default ENet protocol:
```
cmake -S . -B build -DNET_PROTOCOL=WEBSOCKETS && cmake --build build
```
Then you need to use emscripten to build just the client in another directory:
```
emcmake cmake -B build-web && cmake --build build-web
```
Following these commands you will have three programs (excluding tests)
- `build/bomberbloke_server` The server which uses websockets to communicate
- `build/bomberbloke_client` The client which uses websockets to communicate
- `build-web/bomberbloke_client.html` A web page which loads the WASM build of the client, which can connect to the server using websockets

## Client

You can run the client by using the following command.
```
build/bomberbloke_client
```

## Server

The server by default uses the port 8888. You can run the server executable using the following command.
```
build/bomberbloke_server
```
A dockerfile is also available.

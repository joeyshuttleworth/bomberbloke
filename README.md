<<<<<<< HEAD
# Bomberbloke
=======
# Bomberbloke [![Build Status](https://travis-ci.com/joeyshuttleworth/bomberbloke.svg?branch=default)](https://travis-ci.com/joeyshuttleworth/bomberbloke)
>>>>>>> 21b8114b56d35f587b3aac17a2f1161898a411f9

A multiplayer bomberman clone made in C++.

### Prerequisites

#### Dependencies
You will need the dependencies Cereal, SDL2, ENet and curl.

For Ubuntu 20.04:
```
sudo apt-get install libsdl2-dev libsdl2-image-dev libenet-dev libsdl2-mixer-dev libcurl4-openssl-dev cmake make build-essential
```
For MacOS:
```
brew install enet sdl2 sdl2_image sdl2_mixer sdl2_ttf
```

#### Building the BLOKE Engine
Create a build folder and build as follows
```
mkdir build && cd build
cmake ..
make
```


## Client

You can run the client by using the following command.
```
./bomberbloke_client
```

## Server

The server by default uses the port 8888. You can run the server executable using the following command.
```
./bomberbloke_server
```
A dockerfile is also available.

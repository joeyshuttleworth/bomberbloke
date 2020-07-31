# Bomberbloke

A multiplayer bomberman clone made in C++.

### Prerequisites

For Ubuntu 20.04 you will need the dependencies Cereal, SDL2, ENet and curl.

```
sudo apt-get install libsdl2-dev libsdl2-image-dev libenet-dev libsdl2-mixer-dev libcurl4-openssl-dev cmake make build-essential
```

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

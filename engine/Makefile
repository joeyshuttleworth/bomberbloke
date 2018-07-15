all:
	make server
	make client
server:
	gcc -c net.c -o net.o -g -Wall -DIS_SERVER=1
	g++ --std=c++11 -oserver.bin server.cpp engine.cpp actor.cpp level.cpp controls.cpp local_p.cpp player.cpp net.o  -g -I/usr/include/SDL2/ -lSDL2main -lSDL2 -Wall -pthread

client:
	gcc -c net.c -o net.o -g -Wall -DIS_SERVER=0
	g++ --std=c++11 -oclient.bin engine.cpp actor.cpp level.cpp controls.cpp local_p.cpp player.cpp net.o client.cpp -g -I/usr/include/SDL2/ -lSDL2main -lSDL2 -Wall -pthread

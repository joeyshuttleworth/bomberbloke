all:
	gcc -c ../engine/net.c -o net.o
	g++ --std=c++11 -oclient.bin ../bomb.cpp ../bloke.cpp ../engine/engine.cpp ../engine/actor.cpp ../engine/level.cpp ../engine/controls.cpp ../engine/local_p.cpp ../engine/player.cpp net.o client.cpp -g -lSDL2main -lSDL2 -Wall -pthread

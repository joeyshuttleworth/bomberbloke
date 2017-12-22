all:
	gcc -c engine/net.c -g -Wall
	g++ --std=c++11 -obomberbloke.bin bomberbloke.cpp bomb.cpp bloke.cpp engine/engine.cpp engine/actor.cpp engine/level.cpp engine/controls.cpp engine/local_p.cpp engine/player.cpp net.o -g -lSDL2main -lSDL2 -Wall
clean:
	rm *.o
	rm *.bin

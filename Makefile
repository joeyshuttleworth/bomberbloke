all:
	make client

client:
	g++ --std=c++11 -Wall -g  -obomberbloke.bin bomberbloke.cpp  engine/client.cpp bomb.cpp bloke.cpp engine/engine.cpp engine/actor.cpp engine/level.cpp engine/controls.cpp engine/local_p.cpp engine/player.cpp -pthread -Wall -lSDL2main -lSDL2 -I/usr/include/SDL2

clean:
	rm *.o
	rm *.bin

all:
	make client
	make server
server:
	gcc -c engine/net.c -g -Wall -DIS_SERVER=1
	g++ --std=c++11 -Wall -g -obomberbloke_server.bin bomberbloke_server.cpp bomb.cpp bloke.cpp engine/server.cpp engine/engine.cpp engine/actor.cpp engine/level.cpp engine/controls.cpp engine/local_p.cpp engine/player.cpp net.o -pthread -Wall -lSDL2main -lSDL2 -I/usr/include/SDL2
client:
	gcc -c  engine/net.c  -g -Wall -DIS_SERVER=0
	g++ --std=c++11 -Wall -g  -obomberbloke.bin bomberbloke.cpp  engine/client.cpp bomb.cpp bloke.cpp engine/engine.cpp engine/actor.cpp engine/level.cpp engine/controls.cpp engine/local_p.cpp engine/player.cpp net.o -pthread -Wall -lSDL2main -lSDL2 -I/usr/include/SDL2

clean:
	rm *.o
	rm *.bin

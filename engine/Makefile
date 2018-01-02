server:
	gcc -c net.c -g -Wall
	g++ --std=c++11 -D_GLIBCXX_USE_CXX11_ABI=0  -oserver.bin server.cpp  engine.cpp actor.cpp level.cpp controls.cpp local_p.cpp player.cpp net.o -g -pthread -Wall -lSDL2main -lSDL2

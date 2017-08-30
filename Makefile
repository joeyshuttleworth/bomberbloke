all:
	g++ bomberbloke.cpp bomb.cpp bloke.cpp engine/engine.cpp engine/actor.cpp engine/level.cpp -obomberbloke.bin engine/net.cpp engine/controls.cpp engine/local_p.cpp engine/player.cpp -g -lSDL2main -lSDL2 -Wall --std=c++11

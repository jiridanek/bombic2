libs=`sdl-config --libs` -lSDL_ttf -ltinyxml -g
cflags= `sdl-config --cflags ` -g


build: demo2.o SDL_lib.o tixml_helper.o game.o game_mapobjects.o game_AI.o \
	game_background.o game_wall.o game_floorobject.o game_box.o \
	game_bonus.o game_creature.o 
	g++ -o demo2 demo2.o SDL_lib.o tixml_helper.o game.o game_mapobjects.o game_AI.o \
		game_background.o game_wall.o game_floorobject.o\
		game_box.o game_bonus.o game_creature.o ${libs}

SDL_lib.o: src/SDL_lib.cpp src/SDL_lib.h
	g++ -c src/SDL_lib.cpp ${cflags}
tixml_helper.o: src/tixml_helper.cpp src/tixml_helper.h
	g++ -c src/tixml_helper.cpp ${cflags}

game.o: src/game.cpp src/game.h src/game_mapobjects.h src/SDL_lib.h src/constants.h src/tixml_helper.h
	g++ -c src/game.cpp ${cflags}
game_mapobjects.o: src/game_mapobjects.cpp src/game_mapobjects.h src/SDL_lib.h src/constants.h
	g++ -c src/game_mapobjects.cpp ${cflags}
game_AI.o: src/game_AI.cpp src/game_AI.h src/game.h src/game_mapobjects.h src/SDL_lib.h src/constants.h
	g++ -c src/game_AI.cpp ${cflags}

game_background.o: src/game_background.cpp src/game_background.h \
	src/game.h src/game_mapobjects.h src/SDL_lib.h  src/constants.h
	g++ -c src/game_background.cpp ${cflags}
game_wall.o: src/game_wall.cpp src/game_wall.h \
	src/game.h src/game_mapobjects.h src/SDL_lib.h src/constants.h
	g++ -c src/game_wall.cpp ${cflags}
game_floorobject.o: src/game_floorobject.cpp src/game_floorobject.h \
	src/game.h src/game_mapobjects.h src/SDL_lib.h src/constants.h
	g++ -c src/game_floorobject.cpp ${cflags}
game_box.o: src/game_box.cpp src/game_box.h \
	src/game.h src/game_mapobjects.h src/SDL_lib.h src/constants.h
	g++ -c src/game_box.cpp ${cflags}
game_bonus.o: src/game_bonus.cpp src/game_bonus.h \
	src/game.h src/game_mapobjects.h src/SDL_lib.h src/constants.h
	g++ -c src/game_bonus.cpp ${cflags}
game_creature.o: src/game_creature.cpp src/game_creature.h \
	src/game.h src/game_mapobjects.h src/SDL_lib.h src/constants.h src/game_AI.h
	g++ -c src/game_creature.cpp ${cflags}

demo2.o: src/demo2.cpp src/SDL_lib.h src/game.h 
	g++ -c src/demo2.cpp ${cflags}

.PHONY: clean
clean:
	rm -f *.core *.o
.PHONY: run
run: build
	./demo2
.PHONY: deinstall
deinstall:
	rm -f demo2


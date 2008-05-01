
libs=`sdl-config --libs` -lSDL_ttf -ltinyxml -g
cflags= `sdl-config --cflags ` -g


build: demo1.o SDL_lib.o bombic.o
	g++ -o demo1 demo1.o SDL_lib.o bombic.o ${libs}

SDL_lib.o: src/SDL_lib.cpp src/SDL_lib.h
	g++ -c src/SDL_lib.cpp ${cflags}
bombic.o: src/bombic.cpp src/bombic.h src/SDL_lib.h
	g++ -c src/bombic.cpp ${cflags}
demo1.o: src/demo1.cpp src/SDL_lib.h src/bombic.h
	g++ -c src/demo1.cpp ${cflags}

.PHONY: clean
clean:
	rm -f *.core *.o
.PHONY: run
run: build
	./demo1
.PHONY: uninstall
uninstall:
	rm -f demo1

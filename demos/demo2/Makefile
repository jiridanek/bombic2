
LIBS=`sdl-config --libs` -lSDL_ttf  -g
CFLAGS= `sdl-config --cflags ` -g -Wall
CC= g++

TARGET        = bombic2
OBJECTFILES   = $(shell find . -name '*.cpp' | sed 's/.cpp/.o/')
CPPFILES      = $(shell find . -name '*.cpp')


all: $(OBJECTFILES)
	$(CC) -o $(TARGET) $(OBJECTFILES) $(LIBS)


%.o: %.cpp
	$(CC) -c $< $(CFLAGS) -o $@


.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJECTFILES)


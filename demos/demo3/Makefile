
LIBS=`sdl-config --libs` -lSDL_ttf -ltinyxml -g 
CFLAGS= `sdl-config --cflags ` -g -Wall
CC= g++

TARGET        = demo3
OBJECTFILES= $(shell find src -name '*.cpp' | sed 's/.cpp/.o/')
CPPFILES= $(shell find src -name '*.cpp')
HEADERFILES= $(shell find src -name '*.h')

all: $(OBJECTFILES)
	$(CC) -o $(TARGET) $(OBJECTFILES) $(LIBS)

%.o: %.cpp $(HEADERFILES)
	$(CC) -c $< $(CFLAGS) -o $@


.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJECTFILES)


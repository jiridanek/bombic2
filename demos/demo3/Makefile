
LIBS=`sdl-config --libs` -g -lSDL_ttf -ltinyxml 
CFLAGS= `sdl-config --cflags ` -g -Wall #-I./libs/tinyxml
CC= ccache g++

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


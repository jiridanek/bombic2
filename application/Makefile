#DEBUG_FLAGS=-g
LIBS=`agar-config --libs` $(DEBUG_FLAGS) -lSDL_ttf -lSDL_image
CFLAGS= `agar-config --cflags ` $(DEBUG_FLAGS) -Wall -I./libs/tinyxml
CC= ccache g++

TARGET= bombic 
OBJECTFILES= $(shell find src -name '*.cpp' | sed 's/.cpp/.o/')
CPPFILES= $(shell find src -name '*.cpp')
HEADERFILES= $(shell find src -name '*.h')

LIBS_OBJECTFILES= $(shell find libs/tinyxml -name '*.cpp' | sed 's/.cpp/.o/')
LIBS_CPPFILES= $(shell find libs/tinyxml -name '*.cpp')
LIBS_HEADERFILES= $(shell find libs/tinyxml -name '*.h')

PWD=\"$(shell pwd)\"

all: $(OBJECTFILES) $(LIBS_OBJECTFILES)
	$(CC) -o $(TARGET) $(OBJECTFILES) $(LIBS_OBJECTFILES) $(LIBS)

src/%.o: src/%.cpp $(HEADERFILES) $(LIBS_HEADERFILES)
	$(CC) -c $< $(CFLAGS) -o $@ 
libs/%.o: libs/%.cpp $(LIBS_HEADERFILES)
	$(CC) -c $< $(CFLAGS) -o $@

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJECTFILES)
.PHONY: clean_all
clean_all: clean
	rm -f $(LIBS_OBJECTFILES)


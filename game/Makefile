AGAR_CONFIG= agar-config

OPTIMATION=-O3
LIBS=`$(AGAR_CONFIG) --libs` $(OPTIMATION) -lSDL_ttf -lSDL_image
CFLAGS= `$(AGAR_CONFIG) --cflags ` $(OPTIMATION) -Wall -I./libs/tinyxml -I./src -I../common/src $(DEFINES)
#CC= ccache g++
CC= g++

TARGET= bombic 
CPPFILES= $(shell find src -name '*.cpp')
HEADERFILES= $(shell find src -name '*.h')
OBJECTFILES= $(addsuffix .o, $(basename $(CPPFILES)))

LIBS_CPPFILES= $(shell find libs/tinyxml -name '*.cpp')
LIBS_HEADERFILES= $(shell find libs/tinyxml -name '*.h')
LIBS_OBJECTFILES= $(addsuffix .o, $(basename $(LIBS_CPPFILES)))

$(TARGET): $(OBJECTFILES) $(LIBS_OBJECTFILES)
	$(CC) -o $@ $(OBJECTFILES) $(LIBS_OBJECTFILES) $(LIBS)

# Build sources object files
src/%.o: src/%.cpp $(HEADERFILES) $(LIBS_HEADERFILES)
	$(CC) -c $< $(CFLAGS) -o $@ 
# Build libraries object files
libs/%.o: libs/%.cpp $(LIBS_HEADERFILES)
	$(CC) -c $< $(CFLAGS) -o $@

.PHONY: clean distclean

clean:
	rm -f $(TARGET) $(OBJECTFILES)

distclean: clean
	rm -f $(LIBS_OBJECTFILES)


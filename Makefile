#
# Bombic2 main Makefile
#

# For installation
# DESTDIR is for choosing /usr or /usr/local
# DESTDIR is also used when creating a deb package!
DESTDIR=/usr/local

# DATADIR=games  install to  /usr/local/share/games/bombic
# DATADIR=.      install to  /usr/local/share/bombic
DATADIR=games

TARGET=bombic

MAKE_IN=make -C
QT_PROJECT=qmake -project INCLUDEPATH+=../common/src QT+=xml -o bombic-map-editor.pro
QT_MAKEFILE=qmake -makefile bombic-map-editor.pro
CD=cd
DOXYGEN=doxygen

# Phony targets
.PHONY: default all game map-editor clean distclean docs misc

default: game

all: game map-editor

# Build game
game:
	$(MAKE_IN) game

# Build map-editor
map-editor:
	$(CD) map-editor && $(QT_PROJECT) && $(QT_MAKEFILE)
	$(MAKE_IN) map-editor

# Common remote targets
clean distclean: map-editor/Makefile
	$(MAKE_IN) game $@
	$(MAKE_IN) map-editor $@
docs:
	$(CD) game && $(DOXYGEN)
	$(CD) map-editor && $(DOXYGEN)
misc:
	$(MAKE_IN) game/misc
	$(MAKE_IN) map-editor/misc

#### Instalation ####
install: misc
	mkdir -p '$(DESTDIR)/bin'
	mkdir -p '$(DESTDIR)/share/applications' 
	mkdir -p '$(DESTDIR)/share/pixmaps'
	mkdir -p '$(DESTDIR)/share/man/man6'
	mkdir -p '$(DESTDIR)/share/$(DATADIR)/$(TARGET)/common'
	mkdir -p '$(DESTDIR)/share/$(DATADIR)/$(TARGET)/game'
	install -D -m755 game/bombic  '$(DESTDIR)/bin'
	install -D -m644 game/misc/bombic.desktop '$(DESTDIR)/share/applications'
	install -D -m644 game/misc/bombic.png     '$(DESTDIR)/share/pixmaps'
	install -D -m644 game/misc/bombic.6.gz    '$(DESTDIR)/share/man/man6'
	cp -r common/xml common/img '$(DESTDIR)/share/$(DATADIR)/$(TARGET)/common'
	cp -r game/fonts game/misc/bombic.acs '$(DESTDIR)/share/$(DATADIR)/$(TARGET)/game'
	if [ -f map-editor/bombic-map-editor ] ;then \
		install -D -m755 map-editor/bombic-map-editor  '$(DESTDIR)/bin'; \
		install -D -m644 map-editor/misc/bombic-map-editor.desktop '$(DESTDIR)/share/applications'; \
		install -D -m644 map-editor/misc/bombic-map-editor.png     '$(DESTDIR)/share/pixmaps'; \
		install -D -m644 map-editor/misc/bombic-map-editor.6.gz    '$(DESTDIR)/share/man/man6'; \
	fi

uninstall:
	rm -rf '$(DESTDIR)/share/$(DATADIR)/$(TARGET)'
	rm -f  '$(DESTDIR)/bin/bombic'
	rm -f  '$(DESTDIR)/bin/bombic-map-editor'
	rm -f  '$(DESTDIR)/share/applications/bombic.desktop'
	rm -f  '$(DESTDIR)/share/pixmaps/bombic.png'
	rm -f  '$(DESTDIR)/share/man/man6/bombic.6.gz'
	rm -f  '$(DESTDIR)/share/applications/bombic-map-editor.desktop'
	rm -f  '$(DESTDIR)/share/pixmaps/bombic-map-editor.png'
	rm -f  '$(DESTDIR)/share/man/man6/bombic-map-editor.6.gz'


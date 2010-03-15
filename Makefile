#
# Bombic2 main Makefile
#

default: game

MAKE_IN=make -C
QT_PROJECT=qmake -project INCLUDEPATH+=../common/src QT+=xml
QT_MAKEFILE=qmake -makefile
CD=cd
DOXYGEN=doxygen

# Phony targets
.PHONY: default game map-editor clean distclean docs

# Build game
game:
	$(MAKE_IN) game

# Build map-editor
map-editor: map-editor/Makefile
	$(MAKE_IN) map-editor
map-editor/Makefile:
	$(CD) map-editor && $(QT_PROJECT) && $(QT_MAKEFILE)

# Common remote targets
clean distclean: map-editor/Makefile
	$(MAKE_IN) game $@
	$(MAKE_IN) map-editor $@
docs:
	$(CD) game && $(DOXYGEN)
	$(CD) map-editor && $(DOXYGEN)

#
# Bombic2 main Makefile
#

default: game

# Phony targets
.PHONY: default game map-editor clean distclean docs

# Build game
game:
	$(MAKE) -C game

# Build map-editor
map-editor:
	$(MAKE) -C map-editor

# Common remote targets
clean distclean docs:
	$(MAKE) -C game $@
	$(MAKE) -C map-editor $@
	

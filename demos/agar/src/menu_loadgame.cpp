
#include "menu_loadgame.h"

MenuLoadGame::MenuLoadGame(){
	AG_Box * item;

	// nadpis
	createHeading("Load Game");
	// back
	item = createItem("Back");
	AG_SetEvent(item, "window-mousebuttondown", handlerBack, 0);

	AG_SpacerNewHoriz(win);
}

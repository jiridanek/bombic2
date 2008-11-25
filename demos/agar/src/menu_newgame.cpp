
#include "menu_newgame.h"

MenuNewGame::MenuNewGame(){
	AG_Box * item;

	// nadpis
	createHeading("New Game");

	// back
	item = createItem("Back");
	AG_SetEvent(item, "window-mousebuttondown", handlerBack, 0);

	AG_SpacerNewHoriz(win);
}

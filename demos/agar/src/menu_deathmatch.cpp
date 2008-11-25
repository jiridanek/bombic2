
#include "menu_deathmatch.h"

MenuDeathmatch::MenuDeathmatch(){
	AG_Box * item;

	// nadpis
	createHeading("Deathmatch");
	// back
	item = createItem("Back");
	AG_SetEvent(item, "window-mousebuttondown", handlerBack, 0);

	AG_SpacerNewHoriz(win);
}

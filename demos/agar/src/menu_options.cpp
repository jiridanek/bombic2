
#include "menu_options.h"

MenuOptions::MenuOptions(){
	AG_Box * item;

	// nadpis
	createHeading("Options");
	// back
	item = createItem("Back");
	AG_SetEvent(item, "window-mousebuttondown", handlerBack, 0);

	AG_SpacerNewHoriz(win);
}

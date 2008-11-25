
#include "menu_deathmatch_bonuses.h"

int MenuDeathmatchBonuses::bonus = MENU_DEATHMATCH_BONUSES_MIN;

MenuDeathmatchBonuses::MenuDeathmatchBonuses(){
	AG_Box * item;

	// nadpis
	createHeading("Deathmatch bonuses setings");

	// ok
	item = createItem("OK");
	AG_SetEvent(item, "window-mousebuttondown", handlerBack, 0);

	// bonus
	item = createItemHoriz("bonus");
	AG_AddEvent(items_.back(), "window-keydown", handlerIntItem,
		"%p,%i,%i", &bonus,
		MENU_DEATHMATCH_BONUSES_MIN, MENU_DEATHMATCH_BONUSES_MAX);

	AG_NumericalNewIntR(item, 0, 0, 0, &bonus,
		MENU_DEATHMATCH_BONUSES_MIN, MENU_DEATHMATCH_BONUSES_MAX);

	// ok
	item = createItem("OK");
	AG_SetEvent(item, "window-mousebuttondown", handlerBack, 0);

	AG_SpacerNewHoriz(win);
}

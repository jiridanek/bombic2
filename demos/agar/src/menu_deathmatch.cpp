
#include "menu_deathmatch.h"
#include "menu_deathmatch_bonuses.h"
#include "menu_deathmatch_maps.h"

int MenuDeathmatch::players_count = MENU_DEATHMATCH_PLAYERS_MIN;
int MenuDeathmatch::win_points = MENU_DEATHMATCH_WINS_MIN;
int MenuDeathmatch::creatures = 0;
int MenuDeathmatch::bombs_at_end = 0;

MenuDeathmatch::MenuDeathmatch(){
	AG_Box * item;
	AG_Label * label;

	// nadpis
	createHeading("Deathmatch");

	// hrat hru
	item = createItem("Play");
// 	AG_SetEvent(item, "window-mousebuttondown", TODO, 0);

	// map
	item = createItem("Map");
	AG_SetEvent(item, "window-mousebuttondown",
			MenuDeathmatchMaps::create, 0);
	label = AG_LabelNewPolled(item, AG_LABEL_HFILL, "%s",
			MenuDeathmatchMaps::map_name);
	AG_LabelJustify(label, AG_TEXT_CENTER);

	// bonusy
	item = createItem("Bonuses");
	AG_SetEvent(item, "window-mousebuttondown",
			MenuDeathmatchBonuses::create, 0);

	// pocet hracu
	item = createItemHoriz("Players count");
	AG_AddEvent(items_.back(), "window-keydown", handlerIntItem,
		"%p,%i,%i", &players_count,
		MENU_DEATHMATCH_PLAYERS_MIN, MENU_DEATHMATCH_PLAYERS_MAX);

	AG_NumericalNewIntR(item, 0, 0, 0, &players_count,
		MENU_DEATHMATCH_PLAYERS_MIN, MENU_DEATHMATCH_PLAYERS_MAX);

	// pocet vitezstvi
	item = createItemHoriz("Win points");
	AG_AddEvent(items_.back(), "window-keydown", handlerIntItem,
		"%p,%i,%i", &win_points,
		MENU_DEATHMATCH_WINS_MIN, MENU_DEATHMATCH_WINS_MAX);

	AG_NumericalNewIntR(item, 0, 0, 0, &win_points,
		MENU_DEATHMATCH_WINS_MIN, MENU_DEATHMATCH_WINS_MAX);

	// prisery
	item = createItemHoriz("Creatures");
	AG_AddEvent(items_.back(), "window-keydown", handlerBoolItem,
		"%p", &creatures);

	item = AG_BoxNewHoriz(item, AG_BOX_HOMOGENOUS | AG_BOX_HFILL);
	AG_BoxSetPadding(item, 0);
	AG_SpacerNewVert(item);
	AG_CheckboxNewInt(item, 0, "  ", &creatures);

	// bomby na konci kola
	item = createItemHoriz("Bombs at end");
	AG_AddEvent(items_.back(), "window-keydown", handlerBoolItem,
		"%p", &bombs_at_end);

	item = AG_BoxNewHoriz(item, AG_BOX_HOMOGENOUS | AG_BOX_HFILL);
	AG_BoxSetPadding(item, 0);
	AG_SpacerNewVert(item);
	AG_CheckboxNewInt(item, 0, "  ", &bombs_at_end);

	// back
	item = createItem("Back");
	AG_SetEvent(item, "window-mousebuttondown", handlerBack, 0);

	AG_SpacerNewHoriz(win);
}

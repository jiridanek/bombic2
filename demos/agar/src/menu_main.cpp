
#include "menu_main.h"
#include "menu_newgame.h"
#include "menu_loadgame.h"

MenuMain::MenuMain(){
	AG_Box * item;

	// nadpis
	createHeading("Nabídka dne");
	// TODO resume to game

	// new game
	item = createItem("New game");
	AG_SetEvent(item, "window-mousebuttondown", MenuNewGame::create, 0);

	// load game
	item = createItem("Load game");
	AG_SetEvent(item, "window-mousebuttondown", MenuLoadGame::create, 0);

	// TODO save game

	// deathmatch
	item = createItem("Deathmatch");
	AG_SetEvent(item, "window-mousebuttondown", MenuDeathmatch::create, 0);

	// creatures
	item = createItem("Creatures");
	AG_SetEvent(item, "window-mousebuttondown", MenuCreatures::create, 0);
	// bonuses
	item = createItem("Bonuses");
	AG_SetEvent(item, "window-mousebuttondown", MenuBonuses::create, 0);
	// credits
	item = createItem("Credits");
	AG_SetEvent(item, "window-mousebuttondown", MenuCredits::create, 0);

	// options
	item = createItem("Options");
	AG_SetEvent(item, "window-mousebuttondown", MenuOptions::create, 0);

	// quit
	AG_SpacerNewHoriz(win);
	item = createItem("Quit");
	AG_SetEvent(item, "window-mousebuttondown", handlerBack, 0);

	AG_SpacerNewHoriz(win);
}

MenuMain::~MenuMain(){
	AG_Quit();
}

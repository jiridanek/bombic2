
#include "menu_newgame.h"

int MenuNewGame::players_count = MENU_NEWGAME_PLAYERS_MIN;
int MenuNewGame::episode= MENU_NEWGAME_EPISODE_MIN;

MenuNewGame::MenuNewGame(){
	AG_Box * item;

	// nadpis
	createHeading("New Game");

	// hrat hru
	item = createItem("Play");
// 	AG_SetEvent(item, "window-mousebuttondown", TODO, 0);

	// pocet hracu
	item = createItemHoriz("Players count");
	AG_AddEvent(items_.back(), "window-keydown", handlerIntItem,
		"%p,%i,%i", &players_count,
		MENU_NEWGAME_PLAYERS_MIN, MENU_NEWGAME_PLAYERS_MAX);

	AG_NumericalNewIntR(item, 0, 0, 0, &players_count,
		MENU_NEWGAME_PLAYERS_MIN, MENU_NEWGAME_PLAYERS_MAX);

	// epizoda
	item = createItemHoriz("Episode");
	AG_AddEvent(items_.back(), "window-keydown", handlerIntItem,
		"%p,%i,%i", &episode,
		MENU_NEWGAME_EPISODE_MIN, MENU_NEWGAME_EPISODE_MAX);

	AG_NumericalNewIntR(item, 0, 0, 0, &episode,
		MENU_NEWGAME_EPISODE_MIN, MENU_NEWGAME_EPISODE_MAX);


	// back
	item = createItem("Back");
	AG_SetEvent(item, "window-mousebuttondown", handlerBack, 0);

	AG_SpacerNewHoriz(win);
}

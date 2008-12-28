
#include "menu_newgame.h"
#include "game_intro.h"
#include "language.h"

int MenuNewGame::players_count = MENU_NEWGAME_PLAYERS_MIN;
int MenuNewGame::episode= MENU_NEWGAME_EPISODE_MIN;

MenuNewGame::MenuNewGame(){
	AG_Box * item;

	// nadpis
	createHeading(LANG_MENU(LANG_NEW_GAME, LANG_HEADING));

	// hrat hru
	item = createItem(LANG_MENU(LANG_NEW_GAME, LANG_PLAY));
	AG_SetEvent(item, "window-mousebuttondown", handlerNewGame, "%p", this);

	// pocet hracu
	item = createItemHoriz(LANG_MENU(LANG_NEW_GAME, LANG_PLAYERS_COUNT));
	AG_AddEvent(items_.back(), "window-keyup", handlerIntItem,
		"%p,%i,%i", &players_count,
		MENU_NEWGAME_PLAYERS_MIN, MENU_NEWGAME_PLAYERS_MAX);

	AG_NumericalNewIntR(item, 0, 0, 0, &players_count,
		MENU_NEWGAME_PLAYERS_MIN, MENU_NEWGAME_PLAYERS_MAX);

	// epizoda
	item = createItemHoriz(LANG_MENU(LANG_NEW_GAME, LANG_EPISODE));
	AG_AddEvent(items_.back(), "window-keyup", handlerIntItem,
		"%p,%i,%i", &episode,
		MENU_NEWGAME_EPISODE_MIN, MENU_NEWGAME_EPISODE_MAX);

	AG_NumericalNewIntR(item, 0, 0, 0, &episode,
		MENU_NEWGAME_EPISODE_MIN, MENU_NEWGAME_EPISODE_MAX);


	// back
	item = createItem(LANG_MENU(LANG_NEW_GAME, LANG_BACK));
	AG_SetEvent(item, "window-mousebuttondown", handlerBack, 0);

	AG_SpacerNewHoriz(win);
}

extern SDL_Surface * g_window;
extern GameIntro * g_gameIntro;

void MenuNewGame::handlerNewGame(AG_Event * event){
	MenuNewGame * menu = static_cast<MenuNewGame *>(AG_PTR(1));

	try{
		g_gameIntro->new_game(episode, players_count);
		g_gameIntro->show_screen();
	}
	catch(const TiXmlException & ex){
		AG_TextError("%s", ex.what());
	}

	AG_ResizeDisplay(g_window->w, g_window->h);
	menu->show();
}

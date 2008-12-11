
#include <string>
#include "config.h"
#include "game_intro.h"
#include "menu_main.h"
#include "menu_newgame.h"
#include "menu_loadgame.h"
#include "menu_deathmatch.h"
#include "menu_creatures.h"
#include "menu_bonuses.h"
#include "menu_credits.h"
#include "menu_options.h"

extern SDL_Surface * g_window;
extern GameIntro * g_gameIntro;

std::string MenuMain::lang;
bool MenuMain::is_game=false;

MenuMain::MenuMain(){
	lang = CONFIG->language();
	is_game = g_gameIntro && g_gameIntro->is_game();

	AG_Box * item;

	// nadpis
	createHeading("Nabídka dne");
	// TODO resume to game

	// resume game
	if(is_game){
		item = createItem("Resume game");
		AG_SetEvent(item, "window-mousebuttondown",
			handlerResumeGame, "%p", this);
	}

	// new game
	item = createItem("New game");
	AG_SetEvent(item, "window-mousebuttondown", MenuNewGame::create, 0);

	// load game
	item = createItem("Load game");
	AG_SetEvent(item, "window-mousebuttondown", MenuLoadGame::create, 0);

	// save game
	if(is_game){
		item = createItem("Save game");
	// 	AG_SetEvent(item, "window-mousebuttondown", MenuSaveGame::create, 0);
	}

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
	item = createItem("Quit");
	AG_SetEvent(item, "window-mousebuttondown", handlerBack, 0);

	AG_SpacerNewHoriz(win);

	// po zobrazeni zkontrolovat jestli neni treba predelat
	AG_SetEvent(win, "window-shown", handlerMenuShown, 0);
}

void MenuMain::handlerMenuShown(AG_Event * ev){
	if(CONFIG->language()!=lang
	|| (g_gameIntro && g_gameIntro->is_game())!=is_game){
		menu_stack.remove(false);
		create();
		return;
	}
}

void MenuMain::handlerResumeGame(AG_Event * event){
	MenuMain * menu = static_cast<MenuMain *>(AG_PTR(1));

	try{
		g_gameIntro->show_screen();
	}
	catch(const TiXmlException & ex){
		AG_TextError("%s", ex.what());
	}

	AG_ResizeDisplay(g_window->w, g_window->h);
	menu->show();
}


#include <string>
#include <config.h>
#include <language.h>
#include <game/game_intro.h>
#include "main.h"
#include "newgame.h"
#include "load_save.h"
#include "deathmatch.h"
#include "creatures.h"
#include "bonuses.h"
#include "credits.h"
#include "options.h"

extern SDL_Surface * g_window;
extern GameIntro * g_gameIntro;

std::string MenuMain::lang;
bool MenuMain::is_game=false;

MenuMain::MenuMain(){
	lang = CONFIG->language();
	is_game = g_gameIntro && g_gameIntro->is_game();

	AG_Box * item;

	// nadpis
	createHeading(LANG_MENU(LANG_MAIN, LANG_HEADING));

	// resume game
	if(is_game){
		item = createItem(LANG_MENU(LANG_MAIN, LANG_RESUME_GAME));
		AG_SetEvent(item, "window-mousebuttondown",
			handlerResumeGame, "%p", this);
	}

	// new game
	item = createItem(LANG_MENU(LANG_MAIN, LANG_NEW_GAME));
	AG_SetEvent(item, "window-mousebuttondown", MenuNewGame::create, 0);

	// load game
	item = createItem(LANG_MENU(LANG_MAIN, LANG_LOAD_GAME));
	AG_SetEvent(item, "window-mousebuttondown", MenuLoadGame::create, 0);

	// save game
	if(is_game){
		item = createItem(LANG_MENU(LANG_MAIN, LANG_SAVE_GAME));
		AG_SetEvent(item, "window-mousebuttondown", MenuSaveGame::create, 0);
	}

	// deathmatch
	item = createItem(LANG_MENU(LANG_MAIN, LANG_DEATHMATCH));
	AG_SetEvent(item, "window-mousebuttondown", MenuDeathmatch::create, 0);

	/*/ creatures
	item = createItem("Creatures");
	AG_SetEvent(item, "window-mousebuttondown", MenuCreatures::create, 0);
	// */
	/*/ bonuses
	item = createItem("Bonuses");
	AG_SetEvent(item, "window-mousebuttondown", MenuBonuses::create, 0);
	// */
	/*/ credits
	item = createItem("Credits");
	AG_SetEvent(item, "window-mousebuttondown", MenuCredits::create, 0);
	// */

	// options
	item = createItem(LANG_MENU(LANG_MAIN, LANG_OPTIONS));
	AG_SetEvent(item, "window-mousebuttondown", MenuOptions::create, 0);

	// quit
	item = createItem(LANG_MENU(LANG_MAIN, LANG_QUIT));
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

	try{
		g_gameIntro->show_screen();
		handlerMenuShown();
	}
	catch(const TiXmlException & ex){
		AG_TextError("%s", ex.what());
	}

	AG_ResizeDisplay(g_window->w, g_window->h);
}

/** @file menu_load_save.h
 * Soubor obsahuje třídy obsluhující menu pro ukládání a loadování hry.
 */

#ifndef MENU_LOAD_SAVE_H
#define MENU_LOAD_SAVE_H

#include <string>
#include "agar_helper.h"
#include "menu_base.h"

class MenuLoadSaveGame: public MenuBase {
	public:
		virtual ~MenuLoadSaveGame() {};
	protected:
		static void setFilePath(const std::string & filepath);
		static std::string path;
		static std::string filename;
};

class MenuLoadGame: public MenuLoadSaveGame {
	public:
		MenuLoadGame();
		virtual ~MenuLoadGame() {};
		static void create(AG_Event * ev=0)
			{ new MenuLoadGame; }
	private:
		static void handlerLoadGame(AG_Event * event);
};

#define MENU_SAVE_GAME_DEFAULT_FILENAME "saved_game.xml"

class MenuSaveGame: public MenuLoadSaveGame {
	public:
		MenuSaveGame();
		virtual ~MenuSaveGame() {};
		static void create(AG_Event * ev=0)
			{ new MenuSaveGame; }
	private:
		static void handlerSaveGame(AG_Event * event);

};

#endif

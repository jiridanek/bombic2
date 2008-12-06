/** @file menu_loadgame.h
 * Soubor obsahuje třídu obsluhující menu pro loadovani hry.
 */

#ifndef MENU_LOADGAME_H
#define MENU_LOADGAME_H

#include "agar_helper.h"
#include "menu_base.h"

class MenuLoadGame: public MenuBase {
	public:
		MenuLoadGame();
		virtual ~MenuLoadGame() {};
		static void create(AG_Event * ev=0)
			{ new MenuLoadGame; }
	private:
};

#endif

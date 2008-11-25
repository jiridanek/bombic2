/** @file menu_newgame.h
 * Soubor obsahuje třídu obsluhující menu vytvoření nové hry.
 */

#ifndef MENU_NEWGAME_H
#define MENU_NEWGAME_H

#include "agar_helper.h"
#include "menu_base.h"

class MenuNewGame: public MenuBase {
	public:
		MenuNewGame();
		virtual ~MenuNewGame() {};
		static void create(AG_Event * ev=0)
			{ new MenuNewGame; }
	private:
};

#endif

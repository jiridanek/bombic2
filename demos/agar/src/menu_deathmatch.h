/** @file menu_deathmatch.h
 * Soubor obsahuje třídu obsluhující menu vytvoření deathmatche.
 */

#ifndef MENU_DEATHMATCH_H
#define MENU_DEATHMATCH_H

#include "agar_helper.h"
#include "menu_base.h"

class MenuDeathmatch: public MenuBase {
	public:
		MenuDeathmatch();
		virtual ~MenuDeathmatch() {};
		static void create(AG_Event * ev=0)
			{ new MenuDeathmatch; }
	private:
};

#endif

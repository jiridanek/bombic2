/** @file bonuses.h
 * Soubor obsahuje třídu obsluhující modul menu pro zobrazení informací o bonusech.
 */

#ifndef MENU_BONUSES_H
#define MENU_BONUSES_H

#include <helpers/agar.h>
#include "base.h"

class MenuBonuses: public MenuBase {
	public:
		MenuBonuses();
		virtual ~MenuBonuses() {};
		static void create(AG_Event * ev=0)
			{ new MenuBonuses; }
	private:
};

#endif

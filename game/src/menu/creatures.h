/** @file creatures.h
 * Soubor obsahuje třídu obsluhující modul menu pro zobrazení informací o nestvůrách.
 */

#ifndef MENU_CREATURES_H
#define MENU_CREATURES_H

#include <helpers/agar.h>
#include "base.h"

class MenuCreatures: public MenuBase {
	public:
		MenuCreatures();
		virtual ~MenuCreatures() {};
		static void create(AG_Event * ev=0)
			{ new MenuCreatures; }
	private:
};

#endif

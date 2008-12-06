/** @file menu_main.h
 * Soubor obsahuje třídu obsluhující hlavní menu aplikace.
 */

#ifndef MENU_MAIN_H
#define MENU_MAIN_H

#include "agar_helper.h"
#include "menu_base.h"

class MenuMain: public MenuBase {
	public:
		MenuMain();
		virtual ~MenuMain();
		static void create(AG_Event * ev=0)
			{ new MenuMain; }
	private:
};

#endif

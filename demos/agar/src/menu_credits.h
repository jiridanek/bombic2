/** @file menu_credits.h
 * Soubor obsahuje třídu obsluhující modul menu pro zobrazení informaci o autorech.
 */

#ifndef MENU_CREDITS_H
#define MENU_CREDITS_H

#include "agar_helper.h"
#include "menu_base.h"

class MenuCredits: public MenuBase {
	public:
		MenuCredits();
		virtual ~MenuCredits() {};
		static void create(AG_Event * ev=0)
			{ new MenuCredits; }
	private:
};

#endif

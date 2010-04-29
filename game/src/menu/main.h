/** @file main.h
 * Soubor obsahuje třídu obsluhující hlavní menu aplikace.
 */

#ifndef MENU_MAIN_H
#define MENU_MAIN_H

#include <string>
#include <helpers/agar.h>
#include "base.h"

class MenuMain: public MenuBase {
	public:
		MenuMain();
		virtual ~MenuMain() {};
		static void create(AG_Event * ev=0)
			{ new MenuMain; }
	private:
		static void handlerMenuShown(AG_Event * ev=0);
		static void handlerResumeGame(AG_Event * event);
		static std::string lang;
		static bool is_game;
};

#endif

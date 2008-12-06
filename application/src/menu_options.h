/** @file menu_options.h
 * Soubor obsahuje třídu obsluhující menu nastavení hry.
 */

#ifndef MENU_OPTIONS_H
#define MENU_OPTIONS_H

#include "agar_helper.h"
#include "menu_base.h"

class MenuOptions: public MenuBase {
	public:
		MenuOptions();
		virtual ~MenuOptions();
		static void create(AG_Event * ev=0)
			{ new MenuOptions; }
	private:
// 		const char* language;
		int speed_,
			visible_presumption_,
			fullscreen_,
			sound_;

};

#endif

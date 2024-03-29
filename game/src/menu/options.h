/** @file options.h
 * Soubor obsahuje třídu obsluhující menu nastavení hry.
 */

#ifndef MENU_OPTIONS_H
#define MENU_OPTIONS_H

#include <helpers/agar.h>
#include "base.h"

class MenuOptions: public MenuBase {
	public:
		MenuOptions();
		virtual ~MenuOptions();
		static void create(AG_Event * ev=0)
			{ new MenuOptions; }
	private:
		static void handlerLanguage(AG_Event * event);
		static void handlerToggleFullscreen(AG_Event * ev);
		int speed_,
			visible_presumption_,
			split_screen_,
			fullscreen_,
			sound_;

};

#endif

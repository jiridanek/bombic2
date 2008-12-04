/** @file menu_options_controls.h
 * Soubor obsahuje třídu obsluhující menu nestavení kláves.
 */

#ifndef MENU_OPTIONS_CONTROLS_H
#define MENU_OPTIONS_CONTROLS_H

#include "SDL_lib.h"
#include "agar_helper.h"
#include "menu_base.h"

class MenuOptionsControls: public MenuBase {
	public:
		MenuOptionsControls();
		virtual ~MenuOptionsControls() {};
		static void create(AG_Event * ev=0)
			{ new MenuOptionsControls; }
	private:
		static void handlerControls(AG_Event * event);
		static void handlerKeyBack(AG_Event * event);

		Surface background_;

};

#endif

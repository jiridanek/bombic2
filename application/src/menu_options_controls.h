/** @file menu_options_controls.h
 * Soubor obsahuje třídu obsluhující menu nestavení kláves.
 */

#ifndef MENU_OPTIONS_CONTROLS_H
#define MENU_OPTIONS_CONTROLS_H

#include <vector>
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
		static void handlerKeyItem(AG_Event * event);

		Surface background_, background_text_;
		/// Pložku tvoří box a label v něm.
		typedef struct { AG_Box * b; AG_Label * l; } key_item_t;
		/// Pro každého hráče a pro každou akci mám jednu položku.
		typedef std::vector< std::vector< key_item_t > > key_items_t;
		/// Seznam pložek akcí všech hráčů.
		key_items_t key_items_;


};

#endif

/** @file options_controls.h
 * Soubor obsahuje třídu obsluhující menu nestavení kláves.
 */

#ifndef MENU_OPTIONS_CONTROLS_H
#define MENU_OPTIONS_CONTROLS_H

#include <vector>
#include <helpers/sdl.h>
#include <helpers/agar.h>
#include "base.h"

class MenuOptionsControls: public MenuBase {
	public:
		MenuOptionsControls();
		virtual ~MenuOptionsControls() {};
		static void create(AG_Event * ev=0)
			{ new MenuOptionsControls; }
	private:
		static void handlerControls(AG_Event * event);
		static void handlerPlayerItem(AG_Event * event);

		Surface background_, background_text_;
		/// Pložku tvoří box a label v něm.
		typedef struct { AG_Box * b; AG_Label * l; } player_item_t;
		/// Tab tvoří záložka a položky.
		typedef struct { AG_NotebookTab * tab; std::vector< player_item_t > items; } player_tab_t;
		/// Pro každého hráče a pro každou akci mám jednu položku.
		typedef std::vector< player_tab_t > player_tabs_t;
		/// Seznam pložek akcí všech hráčů.
		player_tabs_t player_tabs_;
};

#endif

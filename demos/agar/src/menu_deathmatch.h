/** @file menu_deathmatch.h
 * Soubor obsahuje třídu obsluhující menu vytvoření deathmatche.
 */

#ifndef MENU_DEATHMATCH_H
#define MENU_DEATHMATCH_H

#include "agar_helper.h"
#include "menu_base.h"


#define MENU_DEATHMATCH_PLAYERS_MIN 2
#define MENU_DEATHMATCH_PLAYERS_MAX 4
#define MENU_DEATHMATCH_WINS_MIN 1
#define MENU_DEATHMATCH_WINS_MAX 9

class MenuDeathmatch: public MenuBase {
	public:
		MenuDeathmatch();
		virtual ~MenuDeathmatch() {};
		static void create(AG_Event * ev=0)
			{ new MenuDeathmatch; }
	private:
		static int players_count;
		static int win_points;
		static int creatures;
		static int bombs_at_end;
};

#endif

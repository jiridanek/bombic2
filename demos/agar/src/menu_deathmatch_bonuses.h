/** @file menu_deathmatch_bonuses.h
 * Soubor obsahuje třídu obsluhující menu bonusů deathmatche.
 */

#ifndef MENU_DEATHMATCH_BONUSES_H
#define MENU_DEATHMATCH_BONUSES_H

#include "agar_helper.h"
#include "menu_base.h"

#define MENU_DEATHMATCH_BONUSES_MIN 0
#define MENU_DEATHMATCH_BONUSES_MAX 9

class MenuDeathmatchBonuses: public MenuBase {
	public:
		MenuDeathmatchBonuses();
		virtual ~MenuDeathmatchBonuses() {};
		static void create(AG_Event * ev=0)
			{ new MenuDeathmatchBonuses; }
	private:
		static int bonus;
};

#endif

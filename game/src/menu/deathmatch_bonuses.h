/** @file menu_deathmatch_bonuses.h
 * Soubor obsahuje třídu obsluhující menu bonusů deathmatche.
 */

#ifndef MENU_DEATHMATCH_BONUSES_H
#define MENU_DEATHMATCH_BONUSES_H

#include <helpers/agar.h>
#include <game/deathmatch_intro.h>
#include "base.h"

#define MENU_DEATHMATCH_BONUSES_MIN 0
#define MENU_DEATHMATCH_BONUSES_MAX 9

class MenuDeathmatchBonuses: public MenuBase {
	friend class MenuDeathmatch;
	public:
		MenuDeathmatchBonuses();
		virtual ~MenuDeathmatchBonuses() {};
		static void create(AG_Event * ev=0)
			{ new MenuDeathmatchBonuses; }
	private:
		void load_bonuses_();
		void createBonusItem(AG_Box * parent, int i);
		void createBonusPixmap(AG_Box * parent, int i);

		static DeathmatchIntro::bonuses_t bonuses;
};

#endif

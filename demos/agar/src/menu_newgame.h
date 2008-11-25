/** @file menu_newgame.h
 * Soubor obsahuje třídu obsluhující menu vytvoření nové hry.
 */

#ifndef MENU_NEWGAME_H
#define MENU_NEWGAME_H

#include "agar_helper.h"
#include "menu_base.h"


#define MENU_NEWGAME_PLAYERS_MIN 1
#define MENU_NEWGAME_PLAYERS_MAX 4
#define MENU_NEWGAME_EPISODE_MIN 1
#define MENU_NEWGAME_EPISODE_MAX 2

class MenuNewGame: public MenuBase {
	public:
		MenuNewGame();
		virtual ~MenuNewGame() {};
		static void create(AG_Event * ev=0)
			{ new MenuNewGame; }

		static void handlerPlayersCount(AG_Event * event);
		static void handlerEpisode(AG_Event * event);
	private:
		static int players_count;
		static int episode;
		static const char * episodes[];
};

#endif

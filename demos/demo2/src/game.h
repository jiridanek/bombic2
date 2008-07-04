/** @file game.h
 * Samotná hra.
 * Game.h obsahuje stejnojmennou třídu obstarávající konkrétní hru jako celek
 * a jí podřízené třídy jako například mapObject.
 */
#ifndef GAME_
#define GAME_

#include <iostream>
#include <list>
#include <vector>
#include <string>
#include "SDL_lib.h"
#include "constants.h"
#include "tixml_helper.h"

/** Obecný objekt hry.
 * Čistě virtuální třída zastřešující veškeré objekty v mapě.
 */
class MapObject{
	public:
		/// Vykreslení.
		virtual void draw(SDL_Surface *)=0;
		virtual std::string type()=0;
};

/** Dynamický objekt hry.
 * Objekt v mapě, který během hry vzniká, zaniká
 * nebo se v mapě pohybuje. Má také možnost animace.
 */
class DynamicMO: public MapObject{
	public:
		/// Pohyb.
		virtual void move()=0;
};

/** Statický objekt hry.
 * Objekt v mapě, který je v mapě od začátku do konce hry bez pohybu,
 * pouze s možností animace.
 */
class StaticMO: public MapObject{
	public:
};

/** Konkrétní hra.
 * Instance třídy Game obstarává jednu konkrétní hru.
 * Může to být deathmatch nebo běžný level.
 * Ve svých strukturách uchovává statické i dynamické objekty hry.
 * Stará se o hru samotnou, její spuštění, správné časování a ukončení.
 * Podává informace o typu ukončení hry.
 * @see GameIntro, DeathmatchIntro.
 */
class Game {
	public:
		/// Inicializace hry.
		Game(Uint8 players_count, const std::string & mapname);
// 			bool deathmatch=false, bool creatures=true, bool bombsatend=false);
		/// Uvolnění naalokovaaných surface.
		~Game();
		/// Nastavení parametrů hráče.
		void set_player(Uint8 player_num, Uint8 lives,
			Uint8 bombs, Uint8 flames, Uint8 boots);
		/// Spuštění hry.
		void play();
		void draw(SDL_Surface *window);
		/// Info o ukončení hry.
		bool success() const;
		/// Info o hráči.
		void player(Uint8 player_num, Uint8 & lives,
			Uint8 & bombs, Uint8 & flames, Uint8 & boots) const;
	private:
		/// Seznam dynamických objektů mapy.
		std::list<DynamicMO*> dynamicMOs_;
		/// Seznam statických objektů mapy.
		std::vector<StaticMO*> staticMOs_;
		/// Dvourozměrné pole mapy, na každém políčku seznam objektů na něm položených.
		typedef std::vector< std::vector< std::list< MapObject* > > > map_array_t;
		map_array_t map_array_;

		/// Načtení mapy pro hru.
		void load_map_(const std::string & mapname);
		/// Načtení pozadí mapy.
		void load_background_(const std::string & mapname);
		/// Načtení podelementu pozadí.
		SDL_Surface* load_bg_subEl_(TiXmlElement *bg_el, const char* name_subEl,
				int & toplapping, SDL_Surface* sur_src);
};

#endif

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


/// Všechny typy obejktů
enum OBJECT_TYPES { BACKGROUND, FLOOROBJECT, WALL, BOX, BONUS, CREATURE, PLAYER };

/** Obecný objekt hry.
 * Čistě virtuální třída zastřešující veškeré objekty v mapě.
 */
class MapObject{
	public:
		/// Vykreslení.
		virtual void draw(SDL_Surface *)=0;
		/// Typ objektu.
		virtual OBJECT_TYPES type()=0;
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
		typedef std::list<DynamicMO*> dynamicMOs_t;
		/// Seznam dynamických objektů mapy.
		dynamicMOs_t dynamicMOs_;
		/// Seznam statických objektů mapy.
		std::vector<StaticMO*> staticMOs_;
		/// Dvourozměrné pole mapy, na každém políčku seznam objektů na něm položených.
		typedef std::vector< std::vector< std::list< MapObject* > > > map_array_t;
		map_array_t map_array_;

		/// Načtení mapy pro hru.
		void load_map_(const std::string & mapname);
		/// Načtení pozadí mapy.
		void load_background_(const std::string & bgname);
		/// Načtení zdí mapy.
		void load_walls_(TiXmlElement *wallsEl);
		/// Načtení objektů na zemi.
		void load_floorobjects_(TiXmlElement *floorEl);
		/// Načtení beden mapy.
		void load_boxes_(TiXmlElement *boxesEl);
		/// Načtení bonusů.
		void load_bonuses_(TiXmlElement *bonusEl);
		/// Načtení příšer.
		void load_creatures_(TiXmlElement *creaturesEl);

		/// Načtení surface podelementu.
		SDL_Surface* load_subEl_surface_(TiXmlElement *El, const char* name_subEl,
				int & toplapping, SDL_Surface* sur_src);
		/// Načtení surface souboru v atributu src.
		SDL_Surface* load_src_surface_(TiXmlElement *El, const char* attr_name="src");

		/// Vytvoření a vložení zdi do mapy.
		void insert_wall_(const Surface & sur,
				Uint16 toplapping, Uint16 x, Uint16 y);
		/// Vytvoření a vložení objektu na zemi do mapy.
		void insert_floorobject_(const Surface & sur, Uint16 x, Uint16 y);
		/// Vytvoření a vložení bedny do mapy.
		void insert_box_(const Surface & sur_img, const Surface & sur_burning,
				Uint16 toplapping, Uint16 x, Uint16 y);
		/// Vytvoření a vložení bonusu do mapy.
		void insert_bonus_(const Surface & sur, Uint16 x, Uint16 y);
		/// Vytvoření a vložení příšery do mapy.
		void insert_creature_(const Surface & sur_left, const Surface & sur_left_s,
			const Surface & sur_up, const Surface & sur_up_s,
			const Surface & sur_right, const Surface & sur_right_s,
			const Surface & sur_down, const Surface & sur_down_s,
			const Surface & sur_burned, Uint16 x, Uint16 y);
};

#endif

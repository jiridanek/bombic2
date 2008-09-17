/** @file game.h
 * Samotná hra.
 * Game.h obsahuje stejnojmennou třídu obstarávající konkrétní hru jako celek
 * a jí podřízené třídy jako například mapObject.
 */

#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <list>
#include <vector>
#include <string>
#include "SDL_lib.h"
// #include "constants.h"
#include "game_mapobjects.h"
#include "tixml_helper.h"

/** Konkrétní hra.
 * Instance třídy Game obstarává jednu konkrétní hru.
 * Může to být deathmatch nebo běžný level.
 * Ve svých strukturách uchovává statické i dynamické objekty hry.
 * Stará se o hru samotnou, její spuštění, správné časování a ukončení.
 * Podává informace o typu ukončení hry.
 * @see GameIntro, DeathmatchIntro.
 */
/*
Game::get_instance()->
*/
class Game {
	public:
/*
		static Game *get_instance()
		{
			if (m_game == NULL)
			{
				m_game = new Game;
				if (m_game == NULL) exit(10);
			}
			return m_game;
		}

		static Game *m_game = NULL;
*/
		/// Inicializace hry.
		Game(Uint16 players_count, const std::string & mapname);
// 			bool deathmatch=false, bool creatures=true, bool bombsatend=false);
		/// Uvolnění naalokovaaných surface.
		~Game();
		/// Nastavení parametrů hráče.
		void set_player(Uint16 player_num, Uint16 lives,
			Uint16 bombs, Uint16 flames, Uint16 boots);
		/// TODO Spuštění hry. (zatim jeden tah)
		void play();
		/// Vykreslení scény.
		void draw(SDL_Surface *window);
		/// Info o ukončení hry.
		bool success() const;
		/// Info o hráči.
		void player(Uint16 player_num, Uint16 & lives,
			Uint16 & bombs, Uint16 & flames, Uint16 & boots) const;
		/// Políčko lze přejít.
		static bool field_canGoOver(Uint16, Uint16);
		/// Políčko lze přeletet.
// 		static bool field_canFlyOver(Uint16, Uint16);

		/// Dvourozměrné pole mapy, na každém políčku seznam objektů na něm položených.
		typedef std::vector< std::vector< std::list< MapObject* > > > map_array_t;
		/// Dvourozměrné pole mapy, na každém políčku seznam objektů na něm položených.
		static map_array_t map_array_;

	private:
		typedef std::list<DynamicMO*> dynamicMOs_t;
		/// Seznam dynamických objektů mapy.
		dynamicMOs_t dynamicMOs_;
		/// Seznam statických objektů mapy.
		std::vector<StaticMO*> staticMOs_;

		/// Načtení mapy pro hru.
		void load_map_(Uint16 players_count, const std::string & mapname);
		/// Načtení pozadí mapy.
		void load_background_(const std::string & bgname);
		/// Načtení hráčů.
		void load_players_(TiXmlElement *playersEl, Uint16 count);
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
		/// Načtení surface souboru.
		SDL_Surface* load_src_surface_(TiXmlElement *El,
				const char* attr_name="src", bool force=true);

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
			const Surface & sur_burned, Uint16 x, Uint16 y,
			Uint16 speed, Uint16 lives, Uint16 ai);
		/// Vytvoření a vložení hráče do mapy.
		void insert_player_(const Surface & sur_left, const Surface & sur_left_s,
			const Surface & sur_up, const Surface & sur_up_s,
			const Surface & sur_right, const Surface & sur_right_s,
			const Surface & sur_down, const Surface & sur_down_s,
			const Surface & sur_burned, Uint16 x, Uint16 y,
			Uint16 speed, Uint16 lives);
};



#endif

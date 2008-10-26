/** @file game.h
 * Samotná hra.
 * Game.h obsahuje stejnojmennou třídu obstarávající konkrétní hru jako celek.
 */

#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <list>
#include <vector>
#include <string>
#include <map>
#include "SDL_lib.h"
#include "game_base.h"
#include "game_mapobjects.h"
// #include "tixml_helper.h"


class Player;
class Bomb;

/** Konkrétní hra.
 * Instance třídy Game obstarává jednu konkrétní hru.
 * Může to být deathmatch nebo běžný level.
 * Game může vytvořit pouze jedinou instanci, která je typicky držena
 * z GameIntro nebo DeathmatchIntro. Implementuje nástroje,
 * kterými tuto skutečnost hlídá a poskytuje svůj pointer globálně,
 * prostřednictvím fce Game::get_instance().
 * Ve svých strukturách uchovává statické i dynamické objekty hry.
 * Stará se o hru samotnou, její spuštění, správné časování a ukončení.
 * Podává informace o typu ukončení hry.
 * @see GameIntro, DeathmatchIntro, GameBase.
 */
class Game {
	public:
		/// Získat pointer na jedinou instanci třídy.
		static Game* get_instance();

		/// Inicializace hry.
		Game(const GameBase & base, GameTools * gameTools);
		/// Uvolnění naalokovaaných objektů.
		~Game();
		/// Spuštění hry.
		void play(SDL_Surface *window);
		/// Info o ukončení hry.
		bool success() const;
		/// Info o hráči.
		void player(Uint16 player_num, Uint16 & lives,
			Uint16 & bombs, Uint16 & flames, Uint16 & boots) const;
		/// Políčko lze přejít.
		bool field_canGoOver(Uint16 x, Uint16 y, bool check_bomb=true);
		/// Políčko lze přeletet.
// 		bool field_canFlyOver(Uint16 x, Uint16 y);
		/// Na políčku je typ objektu.
		bool field_withObject(Uint16 x, Uint16 y, const isTypeOf & isType);

		/// Vyhození objektu z mapy.
		void remove_object(DynamicMO * obj);
		/// Vloží objekt na políčko.
		void insert_object(Uint16 x, Uint16 y, DynamicMO * obj);

		/// Pohyb z políčka na políčko.
		void change_position(Uint16 old_x, Uint16 old_y,
			Uint16 new_x, Uint16 new_y, MapObject * obj);

		/// Pro hráče: položit bombu.
		void plant_bomb(Uint16 player_num, Uint16 x, Uint16 y, Bomb* bomb);
		/// Pro hráče: spočítat položené bomby.
		Uint16 count_bombs(Uint16 player_num);
		/// Pro hráče: nechat bouchnout první bombu.
		void explode_bomb(Uint16 player_num);

		/// Výška mapy.
		Uint16 map_height() const;
		/// Šířka mapy.
		Uint16 map_width() const;

		/// Typ dvourozměrného pole mapy, na každém políčku seznam objektů s rozměry.
		typedef std::vector< std::vector< std::list< MapObject* > > > map_array_t;
		/// Typ seznamu dynamickych objektu.
		typedef std::list< DynamicMO* > dynamicMOs_t;
		/// Typ seznamu statickych objektu.
		typedef std::vector< StaticMO* > staticMOs_t;

		/// Pointer pro nadstavbové věci pro hru.
		GameTools * tools;
	private:
		/// Pointer na jedinou instanci třídy.
		static Game* myself_ptr_;
		/// Seznam statických objektů mapy.
		staticMOs_t staticMOs_;
		/// Seznam dynamických objektů mapy.
		dynamicMOs_t dynamicMOs_;
		/// Dvourozměrné pole mapy, na každém políčku seznam objektů na něm položených.
		map_array_t map_array_;

		/// Vykreslení scény.
		void draw_(SDL_Surface *window);
		/// Hýbnutí světem.
		void move_();
		/// Posunutí animací.
		void update_();
		/// Zkopírování pevně umístěných objektů.
		void load_placed_MOs_(const GameBase::base_array_t & base_array);
		/// Vygenerování neumístěných objektů.
		void load_generated_MOs_(const GameBase & base);
		/// Vygenerování beden.
		void generate_boxes_(const GameBase & base,
			GameBase::generatedMOs_t::const_iterator begin,
			GameBase::generatedMOs_t::const_iterator end);
		/// Vygenerování bonusů.
		void generate_bonuses_(
			GameBase::generatedMOs_t::const_iterator begin,
			GameBase::generatedMOs_t::const_iterator end);
		/// Vygenerování příšer.
		void generate_creatures_(const GameBase & base,
			GameBase::generatedMOs_t::const_iterator begin,
			GameBase::generatedMOs_t::const_iterator end);

		/// Vytvoření a vložení objektu do mapy.
		void insert_MO_(const MapObject* mapObject, Uint16 width,
			Uint16 height, Uint16 column, Uint16 field);

		/// Počet žijících příšer.
		Uint16 remaining_creatures_;
		/// Doba do ukončení hry po zabití.
		Uint16 remaining_periods_;
		/// Typ seznamu bomb.
		typedef std::list<Bomb*> bombs_t;
		/// Pointery na hráče a seznam jejich bomb.
		std::map<Uint16, std::pair<Player*, bombs_t> > players_;
};

#endif

/** @file game.h
 * Samotná hra.
 * Game.h obsahuje stejnojmennou třídu obstarávající konkrétní hru jako celek.
 */

#ifndef GAME_H
#define GAME_H

#include <map>
#include <list>
#include <vector>
#include <string>
#include <helpers/sdl.h>
#include <constants.h>
#include <singleton.h>
#include "base.h"
#include "tools.h"
#include "shaker.h"
#include "mapobjects.h"
#include "player.h"
#include "bomb.h"

#define GAME_PLAYER_VIEW_MAX_WIDTH Uint16(16)
#define GAME_PLAYER_VIEW_MAX_HEIGHT Uint16(12)

/** Zkratka pro ziskani pointru na singleton.
 * Makro se expanduje ve volani inline fce, ktera poskytuje pointer
 * na jedinou instanci tridy Game.
 */
#define GAME SINGLETON_SHORTCUT(Game)

/** Konkrétní hra.
 * Instance třídy Game obstarává jednu konkrétní hru.
 * Může to být deathmatch nebo běžný level.
 * Game může vytvořit pouze jedinou instanci, která je typicky držena
 * z GameIntro nebo DeathmatchIntro. Implementuje nástroje,
 * kterými tuto skutečnost hlídá a poskytuje svůj pointer globálně,
 * prostřednictvím makra GAME.
 * Ve svých strukturách uchovává statické i dynamické objekty hry.
 * Stará se o hru samotnou, její spuštění, správné časování a ukončení.
 * Podává informace o typu ukončení hry.
 * @see GameIntro, DeathmatchIntro, GameBase.
 */
class Game {

	SINGLETON_DEFINITION(Game)

	public:
		/// Inicializace hry.
		Game(const GameBase & base, GameTools * gameTools,
				bool deathmatch=false, bool bombsatend=false);
		/// Uvolnění naalokovaaných objektů.
		~Game();
		/// Spuštění hry.
		void play(SDL_Surface *window);
		/// Info o ukončení hry.
		bool success() const;
		/// Info o hráči.
		bool get_player(Uint16 player_num, PlayerProperties & prop) const;
		/// Číslo hráče, který je živý.
		Uint16 winner();
		/// Políčko lze přejít.
		bool field_canGoOver(Uint16 x, Uint16 y, bool check_bomb=true);
		/// Políčko lze přejít.
		bool field_canGoOver(const field_t & field, bool check_bomb=true){
			return field_canGoOver(field.first, field.second, check_bomb);
		}
		/// Políčko lze přeletet.
// 		bool field_canFlyOver(Uint16 x, Uint16 y);
		/// Na políčku je typ objektu.
		bool field_withObject(Uint16 x, Uint16 y, const isTypeOf & isType);
		/// Na políčku je typ objektu.
		bool field_withObject(const field_t & field, const isTypeOf & isType){
			return field_withObject(field.first, field.second, isType);
		}
		/// První objekt zadaného typu.
		MapObject * field_getObject(Uint16 x, Uint16 y, const isTypeOf & isType);
		/// První objekt zadaného typu.
		MapObject * field_getObject(const field_t & field, const isTypeOf & isType){
			return field_getObject(field.first, field.second, isType);
		}

		/// Vyhození objektu z mapy.
		void remove_object(DynamicMO * obj);
		/// Vloží objekt na políčko.
		void insert_object(Uint16 x, Uint16 y, DynamicMO * obj);
		/// Vloží objekt na políčko.
		void insert_object(const field_t & field, DynamicMO * obj){
			insert_object(field.first, field.second, obj);
		}

		/// Pohyb z políčka na políčko.
		void change_position(Uint16 old_x, Uint16 old_y,
			Uint16 new_x, Uint16 new_y, MapObject * obj);
		/// Pohyb z políčka na políčko.
		void change_position(const field_t & old_field,
				const field_t & new_field, MapObject * obj){
			change_position(
				old_field.first, old_field.second,
				new_field.first, new_field.second, obj);
		}

		/// Pro hráče: položit bombu.
		void plant_bomb(Uint16 player_num,
				Uint16 x, Uint16 y, Bomb* bomb);
		/// Pro hráče: položit bombu.
		void plant_bomb(Uint16 player_num,
				const field_t & field, Bomb* bomb){
			plant_bomb(player_num,
				field.first, field.second, bomb);
		}
		/// Pro hráče: spočítat položené bomby.
		Uint16 count_bombs(Uint16 player_num);
		/// Pro hráče: odstranit ruční odpalování u bomb.
		void remove_bombs_timer(Uint16 player_num);
		/// Pro hráče: nechat bouchnout první bombu.
		void explode_bomb(Uint16 player_num);
		/// Pro hráče (nebo spíše bonusy): nechat bouchnout první bombu.
		void add_others_bonus(Uint16 player_num, const char * bonus_name);

		/// Třesení pohledy
		void shake_views();

		/// Spustit třesení pohledem
		void start_view_shaking(Uint16 player_num);

		/// Zastavit třesení pohledem
		void stop_view_shaking(Uint16 player_num);

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
		/// Seznam statických objektů mapy.
		staticMOs_t staticMOs_;
		/// Seznam dynamických objektů mapy.
		dynamicMOs_t dynamicMOs_;
		/// Dvourozměrné pole mapy, na každém políčku seznam objektů na něm položených.
		map_array_t map_array_;



		/// Nastavení obdélníku jako jedíného pohledu na obrazovce.
		void set_one_win_view_(SDL_Rect & win_view, SDL_Surface * window);

		/// Zkontrolovat není-li třeba rozdělit či sloučit obrazovku.
		void check_players_view_(SDL_Surface* window);

		/// Nastavení pohledů pro hráče.
		void set_players_view_(SDL_Surface *window);
		/// Nastavení pohledů - jeden pohled na obrazovce.
		void set_players_view_1_(SDL_Surface * window);
		/// Nastavení pohledů - dva pohledy na obrazovce.
		void set_players_view_2_(SDL_Surface * window);
		/// Nastavení pohledů - 3 nebo 4 pohledy na obrazovce.
		void set_players_view_4_(SDL_Surface * window);

		/// Spočítá posun mapy podle pozice hráče.
		Uint16 count_rect_shift_(Uint16 player_coor,
				Uint16 rect_half_size, Uint16 map_size) const;
		void count_players_average_(Sint16 & shift_x, Sint16 & shift_y);

		/// Vykreslení obrazovky.
		void draw_(SDL_Surface * window);
		/// Vykreslení pohledu hráče.
		void draw_players_view_(SDL_Surface *window, Uint16 player_num);
		/// Vykreslení jednoho pohledu pro více hráčů.
		void draw_one_view_(SDL_Surface * window);
		/// Vykreslení části mapy.
		void draw_map_(bool bg, SDL_Surface* window, SDL_Rect & map_view,
			Uint16 from_x, Uint16 from_y, Uint16 to_x, Uint16 to_y );
		/// Vykreslení pozadí jednoho políčka mapy.
		void draw_map_field_bg_(SDL_Surface* window, SDL_Rect & map_view,
			Uint16 column, Uint16 field);
		/// Vykreslení popředí jednoho políčka mapy.
		void draw_map_field_fg_(SDL_Surface* window, SDL_Rect & map_view,
			Uint16 column, Uint16 field);

		/// Hýbnutí světem.
		bool move_();
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
		/// Jedná se o deathmatch.
		bool deathmatch_, bombsatend_; ///< Násilné ukončení hry.

		/// Typ seznamu bomb.
		typedef std::list<Bomb*> bombs_t;
		/// Typ hráče s jeho pohledy (na okno a na mapu),
		/// třesením pohledu a položenými bombami.
		typedef struct{
				Player* player;
				SDL_Rect win_view, map_view;
				GameShaker shaker;
				bombs_t bombs;
			} player_t;
		/// Typ seznamu hráčů hry, který umí rychle najít hráče podle čísla.
		typedef std::map< Uint16, player_t > players_t;
		/// Typ iterátoru do seznamu hráčů.
		typedef players_t::iterator players_it;
		/// Hráči hry.
		players_t players_;

		/// Jestli je aktualne rozdelena obrazovka.
		bool split_screen_;
};

#endif

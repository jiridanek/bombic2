/** @file game_base.h
 * Nahrávání hry z xml souborů.
 * Game_base.h obsahuje třídy připravující hru a zejména mapu hry z xml souborů.
 * Postup vytvoření hry je takový, že se jednou naloadují struktury z xml souborů,
 * pevně umístěné objekty se umístí do mapy, náhodně umísťované objekty se připraví mimo.
 * Až poté se z této předpřipravené struktury vygeneruje hra a její mapa
 * pro každou jednu hru (od narození po úmrtí), nově od demo2 se také řeší prázdná políčka.
 */

#ifndef GAME_BASE_H
#define GAME_BASE_H

#include <iostream>
#include <list>
#include <vector>
#include <string>
#include "SDL_lib.h"
#include "game_mapobjects.h"
#include "tixml_helper.h"

/** Základ hry.
 * Třída GameBase řeší první načtení mapy a jejích objektů z XML,
 * připraví hrací pole a vloží do něj (a nikam jinam) pevně umístěné objekty.
 * Náhodně generované objekty (statické, respektive dynamické) se uloží do
 * seznamů statických, respektive dynamických objektů.
 * TODO bomb, flame, icons atd.
 * Z této předpřipravené struktury se vytvoří konkrétní hra, proto je friend s třídou Game.
 * @see GameIntro, DeathmatchIntro.
 */
class GameBase {
	friend class Game;
	public:
		/// Inicializace hry.
		GameBase(Uint16 players_count, const std::string & mapname);
// 			bool deathmatch=false, bool creatures=true, bool bombsatend=false);
		/// Uvolnění naalokovaaných objektů.
		~GameBase();
		/// Nastavení parametrů hráče.
		void set_player(Uint16 player_num, Uint16 lives,
			Uint16 bombs, Uint16 flames, Uint16 boots);
		typedef struct { MapObject* o; Uint16 w, h; } proportionedMO_t;
		/// Typ dvourozměrného pole mapy, na každém políčku seznam objektů s rozměry.
		typedef std::vector< std::vector< std::list< proportionedMO_t > > > base_array_t;
		/// Typ dvourozměrného pole, na každém políčku TRUE, pokud je možné vygenerovat box.
		typedef std::vector< std::vector< bool> > boxes_array_t;
		/// Typ seznamu generovaných objektů.
		typedef std::vector< MapObject* > generatedMOs_t;
	private:
		/// Pole mapy.
		base_array_t base_array_;
		/// Pole mapy, na každém políčku TRUE, pokud je možné vygenerovat box.
		boxes_array_t boxes_array_;
		/// Počet volných políček v boxes_array_.
		Uint16 boxes_array_count_;
		/// Seznam neumístěných objektů.
		generatedMOs_t generatedMOs_;

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

		/// Načtení animace podelementu.
		Uint16 load_subEl_animation_(TiXmlElement *El, const char* name_subEl,
				Animation & anim, const Surface & sur_src);
		/// Načtení surface bitmapy.
		SDL_Surface* load_src_surface_(TiXmlElement *El,
				const char* attr_name="src", bool force=true);

		/// Vytvoření a vložení pozadí do mapy.
		void insert_background_(const Animation & anim,
				const Animation & anim_burned, Uint16 x, Uint16 y);
		/// Vytvoření a vložení zdi do mapy.
		void insert_wall_(const Animation & anim,
				Uint16 toplapping, Uint16 x, Uint16 y, Uint16 w, Uint16 h);
		/// Vytvoření a vložení objektu na zemi do mapy.
		void insert_floorobject_(const Animation & anim,
				Uint16 x, Uint16 y, Uint16 w, Uint16 h);
		/// Pouze vytvoření bedny.
		void insert_box_(const Animation & anim, const Animation & sur_burning,
				Uint16 toplapping);
		/// Vytvoření a vložení bedny do mapy.
		void insert_box_(const Animation & anim, const Animation & sur_burning,
				Uint16 toplapping, Uint16 x, Uint16 y, Uint16 w, Uint16 h);
		/// Pouze vytvoření bonusu.
		void insert_bonus_(const Animation & anim);
		/// Pouze vytvoření příšery.
		void insert_creature_(const Animation & anim_up, const Animation & anim_right,
			const Animation & anim_down, const Animation & anim_left,
			const Animation & anim_burned, Uint16 speed, Uint16 lives, Uint16 ai);
		/// Vytvoření a vložení příšery do mapy.
		void insert_creature_(const Animation & anim_up, const Animation & anim_right,
			const Animation & anim_down, const Animation & anim_left,
			const Animation & anim_burned, Uint16 x, Uint16 y,
			Uint16 speed, Uint16 lives, Uint16 ai);
		/// Vytvoření a vložení hráče do mapy.
		void insert_player_(const Animation & anim_up, const Animation & anim_right,
			const Animation & anim_down, const Animation & anim_left,
			const Animation & anim_burned, Uint16 x, Uint16 y,
			Uint16 speed, Uint16 lives);
		/// Vyhození nulových pointerů z base_array_.
		void clear_null_objects_();
		/// Odalokování všech zde vytvořených objektů.
		void destroy_();
};

#endif

/** @file game/base.h
 * Nahrávání hry z xml souborů.
 * Obsahuje třídy @c GameBaseLoader a @c GameBase připravující hru a zejména mapu hry z xml souborů.
 * Postup vytvoření hry je takový, že se jednou naloadují struktury z xml souborů,
 * pevně umístěné objekty se umístí do mapy, náhodně umísťované objekty se připraví mimo.
 * Až poté (a to už se řeěí v game_intro.h) se z této předpřipravené struktury vygeneruje hra a její mapa
 * pro každou jednu hru (od narození po úmrtí).
 */

#ifndef GAME_BASE_H
#define GAME_BASE_H

// #include <iostream>
#include <list>
#include <vector>
#include <string>
#include <helpers/sdl.h>
#include <helpers/tixml.h>
#include "mapobjects.h"
#include "player.h"

/** Loader animace a surface z xml.
 * Pomocná třída, která implementuje obecné fce pro loadování
 * animace z elementu a zdrojového surface z atributu elementu.
 */
class GameBaseLoader {
	public:
		/// Načtení surface podelementu.
		static void load_subEl_surface(TiXmlElement *El,
				const char* name_subEl, Surface & sur_dst,
				Uint16 width, Uint16 height,
				const Surface & sur_src);
		/// Načtení animace podelementu.
		static Uint16 load_subEl_animation(TiXmlElement *El,
				const char* name_subEl,
				Animation & anim_dst, const Surface & sur_src);
		/// Načtení surface bitmapy.
		static SDL_Surface* load_src_surface(TiXmlElement *El,
				const char* attr_name="src", bool force=true);
};

/** Základ hry.
 * Třída GameBase řeší první načtení mapy a jejích objektů z XML,
 * připraví hrací pole a vloží do něj (a nikam jinam) pevně umístěné objekty.
 * Náhodně generované objekty (statické, respektive dynamické) se uloží do
 * seznamů statických, respektive dynamických objektů.
 * Bomby, plameny a ikony se zde nepřipravují, nejsou základními objekty mapy.
 * Z této předpřipravené struktury se vytvoří konkrétní hra, proto je friend s třídou Game.
 * @see GameIntro, DeathmatchIntro, GameTools.
 */
class GameBase: public GameBaseLoader {
	friend class Game;
	public:
		typedef struct { std::string n; int c;} bonus_t;
		/// Typ seznamu jmen a počtů bonusů.
		typedef std::vector< bonus_t > bonuses_t;

		/// Inicializace normální hry.
		GameBase(Uint16 players_count, const std::string & mapname);
		/// Inicializace hry pro deathmatch.
		GameBase(Uint16 players_count, const std::string & mapname,
			const bonuses_t & bonuses, bool creatures);
		/// Uvolnění naalokovaaných objektů.
		~GameBase();
		/// Nastavení parametrů hráče.
		void set_player(Uint16 player_num, const PlayerProperties & prop);
		/// Získání parametrů hráče.
		void get_player(Uint16 player_num, PlayerProperties & prop);

		typedef struct { MapObject* o; Uint16 w, h; } proportionedMO_t;
		/// Typ dvourozměrného pole mapy, na každém políčku seznam objektů s rozměry.
		typedef std::vector< std::vector< std::list< proportionedMO_t > > > base_array_t;
		typedef struct { bool box, creature; } allowed_field_t;
		/// Typ dvourozměrného pole, na každém políčku TRUE, pokud je možné vygenerovat box.
		typedef std::vector< std::vector< allowed_field_t> > allowed_array_t;
		/// Typ seznamu generovaných objektů.
		typedef std::vector< MapObject* > generatedMOs_t;

	private:
		/// Pole mapy.
		base_array_t base_array_;
		/// Pole mapy, na každém políčku TRUE, pokud je možné vygenerovat box.
		allowed_array_t allowed_array_;
		/// Počet volných políček v boxes_array_.
		Uint16 allowed_boxes_count_;
		/// Seznam neumístěných objektů.
		generatedMOs_t generatedMOs_;
		/// Seznam hráčů.
		std::vector< Player* > players_;

		/// Načtení mapy pro hru.
		void load_map_(Uint16 players_count, const std::string & mapname,
			bool deathmatch, bool creatures);
		/// Načtení pozadí mapy.
		void load_background_(const std::string & bgname);
		/// Načtení hráčů.
		void load_players_(
				TiXmlElement *playersEl, Uint16 count);
		/// Načtení hráčů do deathmatche.
		void load_players_deathmatch_(
				TiXmlElement *playersEl, Uint16 count);
		/// Načtení zdí mapy.
		void load_walls_(TiXmlElement *wallsEl);
		/// Načtení objektů na zemi.
		void load_floorobjects_(TiXmlElement *floorEl);
		/// Načtení beden mapy.
		void load_boxes_(TiXmlElement *boxesEl);
		/// Připravení políček, na kterých nesmí být bedna.
		void load_noboxes_(TiXmlElement *boxesEl);
		/// Načtení bonusů.
		void load_bonuses_(TiXmlElement *bonusEl);
		/// Načtení bonusů z externího zdroje.
		void load_bonuses_(const bonuses_t & bonuses);
		/// Načtení příšer.
		void load_creatures_(TiXmlElement *creaturesEl);
		/// Připravení políček, na kterých nesmí být příšery.
		void load_nocreatures_(TiXmlElement *nocreaturesEl);

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
		void insert_box_(
				const Animation & anim, const Animation & sur_burning,
				Uint16 toplapping);
		/// Vytvoření a vložení bedny do mapy.
		void insert_box_(
				const Animation & anim, const Animation & sur_burning,
				Uint16 toplapping, Uint16 x, Uint16 y, Uint16 w, Uint16 h);
		/// Pouze vytvoření bonusu.
		void insert_bonus_(const std::string & bonus_name,
				const Animation & anim);
		/// Pouze vytvoření příšery.
		void insert_creature_(
				const Animation & anim_up, const Animation & anim_right,
				const Animation & anim_down, const Animation & anim_left,
				const Animation & anim_burned,
				Uint16 speed, Uint16 lives, Uint16 ai);
		/// Vytvoření a vložení příšery do mapy.
		void insert_creature_(
				const Animation & anim_up, const Animation & anim_right,
				const Animation & anim_down, const Animation & anim_left,
				const Animation & anim_burned, Uint16 x, Uint16 y,
				Uint16 speed, Uint16 lives, Uint16 ai);
		/// Vytvoření a vložení hráče do mapy.
		void insert_player_(
				const Animation & anim_up, const Animation & anim_right,
				const Animation & anim_down, const Animation & anim_left,
				const Animation & anim_burned, Uint16 x, Uint16 y,
				Uint16 speed, Uint16 num);
		/// Vyhození nulových pointerů z base_array_.
		void clear_null_objects_();
};

#endif

/** @file game_base.h
 * Nahrávání hry z xml souborů.
 * Game_base.h obsahuje třídy připravující hru a zejména mapu hry z xml souborů.
 * Postup vytvoření hry je takový, že se jednou naloadují struktury z xml souborů,
 * pevně umístěné objekty se umístí do mapy, náhodně umísťované objekty se připraví mimo.
 * Až poté (a to už se řeěí v game_intro.h) se z této předpřipravené struktury vygeneruje hra a její mapa
 * pro každou jednu hru (od narození po úmrtí).
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


/** Loader animace a surface z xml.
 * Pomocná třída, která implementuje obecné fce pro loadování
 * animace z elementu a zdrojového surface z atributu elementu.
 */
class GameBaseLoader {
	protected:
		/// Načtení surface podelementu.
		void load_subEl_surface_(TiXmlElement *El, const char* name_subEl,
				Surface & sur_dst, Uint16 width, Uint16 height, const Surface & sur_src);
		/// Načtení animace podelementu.
		Uint16 load_subEl_animation_(TiXmlElement *El, const char* name_subEl,
				Animation & anim_dst, const Surface & sur_src);
		/// Načtení surface bitmapy.
		SDL_Surface* load_src_surface_(TiXmlElement *El,
				const char* attr_name="src", bool force=true);
};

/** Základ hry.
 * Třída GameBase řeší první načtení mapy a jejích objektů z XML,
 * připraví hrací pole a vloží do něj (a nikam jinam) pevně umístěné objekty.
 * Náhodně generované objekty (statické, respektive dynamické) se uloží do
 * seznamů statických, respektive dynamických objektů.
 * TODO bomb, flame, icons atd.
 * Z této předpřipravené struktury se vytvoří konkrétní hra, proto je friend s třídou Game.
 * @see GameIntro, DeathmatchIntro.
 */
class GameBase: public GameBaseLoader {
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
		/// Připravení políček, na kterých nesmí být bedna.
		void load_noboxes_(TiXmlElement *boxesEl);
		/// Načtení bonusů.
		void load_bonuses_(TiXmlElement *bonusEl);
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
			Uint16 speed, Uint16 lives, Uint16 num);
		/// Vyhození nulových pointerů z base_array_.
		void clear_null_objects_();
		/// Odalokování všech zde vytvořených objektů.
		void destroy_();
};


class Flame;
class Bomb;
class Presumption;

/** Nadstavbové věci pro hru.
 * Třída GameTools naloaduje z XML věci potřebné ke hře
 * a poskytuje je v podobě jednoduchého interface.
 */
class GameTools: public GameBaseLoader{
	public:
		GameTools();
		Flame* flame_top(Uint16 x, Uint16 y) const;
		Flame* flame_bottom(Uint16 x, Uint16 y) const;
		Flame* flame_topbottom(Uint16 x, Uint16 y) const;
		Flame* flame_left(Uint16 x, Uint16 y) const;
		Flame* flame_right(Uint16 x, Uint16 y) const;
		Flame* flame_leftright(Uint16 x, Uint16 y) const;
		Flame* flame_cross(Uint16 x, Uint16 y) const;
		Bomb* bomb_normal(Uint16 x, Uint16 y, Uint16 flamesize) const;
		Bomb* bomb_mega(Uint16 x, Uint16 y, Uint16 flamesize) const;
		Presumption* presumption(Uint16 x, Uint16 y) const;

		void draw_panel_player(SDL_Surface * window,
			Uint16 player_num, Uint16 flames, Uint16 bombs,
			Uint16 megabombs, bool slider, bool kicker);
		void draw_panel_bonus(SDL_Surface * window,
			Uint16 player_num, BONUSES bonus,
			const std::string & val = "");

		enum BONUSES { MEGABOMB, SHIELD, ILLNESS, TIMER, FIREMAN, SLIDER, KICKER };
		#define GAMETOOLS_BONUSES_NAMES \
		{ "megabomb", "shield", "illness", "timer", "fireman", "slider","kicker" }
		#define GAMETOOLS_BONUSES_COUNT 7
		#define GAMETOOLS_BONUSES_FONT_SIZE 10
	private:
		void load_flame_(TiXmlElement *flameEl, const Surface & sur_src);
		void load_bombs_(TiXmlElement *bombsEl, const Surface & sur_src);
		void load_panels_(TiXmlElement *panelsEl, const Surface & sur_src);
		void load_bonuses_(TiXmlElement *bonusesEl, const Surface & sur_src);

		Uint16 flame_period_;
		Animation flame_top_, flame_bottom_, flame_topbottom_,
			flame_left_, flame_right_, flame_leftright_, flame_cross_;
		Animation bomb_normal_, bomb_mega_;
		Animation presumption_;

		Surface panel_pl1_, panel_pl2_, panel_pl3_, panel_pl4_;

		typedef struct { Surface sur; Uint16 x,y; } positioned_surface_t;
		std::vector< positioned_surface_t > panels_;
		std::vector< positioned_surface_t > bonuses_;
};

#endif

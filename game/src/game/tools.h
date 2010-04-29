/** @file tools.h
 * Nahrávání věcí pro hru z xml souborů.
 * Obsahuje třídu GameTools připravující věci pro hru z xml souborů.
 */

#ifndef GAME_TOOLS_H
#define GAME_TOOLS_H

#include <vector>
#include <string>
#include <map>
#include <helpers/sdl.h>
#include <helpers/tixml.h>
#include "base.h"
#include "mapobjects.h"

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

		Uint16 getFlamePeriod();

		Flame* flame_top(Uint16 x, Uint16 y) const;
		Flame* flame_top(const field_t & field) const {
			return flame_top(field.first, field.second);
		}
		Flame* flame_bottom(Uint16 x, Uint16 y) const;
		Flame* flame_bottom(const field_t & field) const {
			return flame_bottom(field.first, field.second);
		}
		Flame* flame_topbottom(Uint16 x, Uint16 y) const;
		Flame* flame_topbottom(const field_t & field) const {
			return flame_topbottom(field.first, field.second);
		}
		Flame* flame_left(Uint16 x, Uint16 y) const;
		Flame* flame_left(const field_t & field) const {
			return flame_left(field.first, field.second);
		}
		Flame* flame_right(Uint16 x, Uint16 y) const;
		Flame* flame_right(const field_t & field) const {
			return flame_right(field.first, field.second);
		}
		Flame* flame_leftright(Uint16 x, Uint16 y) const;
		Flame* flame_leftright(const field_t & field) const {
			return flame_leftright(field.first, field.second);
		}
		Flame* flame_cross(Uint16 x, Uint16 y) const;
		Flame* flame_cross(const field_t & field) const {
			return flame_cross(field.first, field.second);
		}
		Bomb* bomb_normal(Uint16 x, Uint16 y,
				Uint16 flamesize, bool timer=false) const;
		Bomb* bomb_normal(const field_t & field,
				Uint16 flamesize, bool timer=false) const {
			return bomb_normal(field.first, field.second,
				flamesize, timer);
		}
		Bomb* bomb_mega(Uint16 x, Uint16 y,
				Uint16 flamesize, bool timer=false) const;
		Bomb* bomb_mega(const field_t & field,
				Uint16 flamesize, bool timer=false) const {
			return bomb_mega(field.first, field.second,
				flamesize, timer);
		}
		Presumption* presumption(Uint16 x, Uint16 y,
				Uint16 to_end);
		Presumption* presumption(const field_t & field,
				Uint16 to_end) {
			return presumption(field.first, field.second,
				to_end);
		}

		enum BONUSES {
				MEGABOMB, TIMER, SHIELD, ILLNESS,
				FIREMAN, SLIDER, KICKER };
		#define GAMETOOLS_BONUSES_NAMES \
				{ "megabomb", "timer", "shield", "illness",\
				  "fireman", "slider", "kicker" }
		#define GAMETOOLS_BONUSES_COUNT 7
		#define GAMETOOLS_BONUSES_FONT_SIZE 10

		void draw_panel_player(SDL_Surface * window, const SDL_Rect & rect,
			Uint16 player_num, Uint16 lives, Uint16 flames, Uint16 bombs,
			Uint16 megabombs, bool slider, bool kicker);
		void draw_panel_bonus(SDL_Surface * window, const SDL_Rect & rect,
			Uint16 player_num, BONUSES bonus,
			const std::string & val = "");
	private:
		void load_flame_(TiXmlElement *flameEl,
				const Surface & sur_src);
		void load_bombs_(TiXmlElement *bombsEl,
				const Surface & sur_src);
		void load_panels_(TiXmlElement *panelsEl, const Surface & sur_src);
		void load_bonuses_(TiXmlElement *bonusesEl,
				const Surface & sur_src);
		Uint16 get_panel_x_(Uint16 player_num, const SDL_Rect & rect);
		Uint16 get_panel_y_(Uint16 player_num, const SDL_Rect & rect);

		Uint16 flame_period_, bomb_speed_;
		Animation flame_top_, flame_bottom_, flame_topbottom_,
			flame_left_, flame_right_, flame_leftright_, flame_cross_;
		Animation bomb_normal_, bomb_mega_;
		Animation presumption_;

		typedef struct { Surface sur; Uint16 x,y; } positioned_surface_t;
		std::vector< positioned_surface_t > panels_;
		std::vector< positioned_surface_t > bonuses_;

		typedef std::map< std::string, Surface > texts_t;
		texts_t texts_;
		const Surface & text_(const std::string & text);
};

#endif

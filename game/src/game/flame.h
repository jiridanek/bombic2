/** @file game_flame.h
 * Plamen.
 * Game_flame.h obsahuje třídu obstarávající plamen bomby.
 */
#ifndef GAME_FLAME_H
#define GAME_FLAME_H

#include <helpers/sdl.h>
#include "mapobjects.h"


/** Plamen bomby.
 * Dynamický objekt vytvářený především bombou.
 * Zabíjí, sbírá (spaluje) bonusy, nehýbe se.
 */
class Flame: public DynamicMO {
	public:
		/// Vytvoří plamen.
		Flame(const Animation & anim, Uint16 x, Uint16 y, Uint16 period);
		/// Pohyb.
		virtual bool move();
		/// Vykreslení.
		virtual void draw(SDL_Surface *window, const SDL_Rect & rect);
		/// Posun animace.
		virtual void update();
		/// Typ objektu je plamen.
		virtual OBJECT_TYPES type() const { return FLAME; }
		/// Destructor
		virtual ~Flame() {};
	protected:
		/// Animace.
		Animation anim_;
		/// Počítadlo přístupů.
		Uint16 access_counter_,
		/// Délka života plamene.
			period_;
};

#endif

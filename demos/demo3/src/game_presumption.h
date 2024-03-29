/** @file game_presumption.h
 * Předvídání plamene.
 * Game_presumption.h obsahuje třídu obstarávající předvídání plamene.
 */
#ifndef GAME_PRESUMPTION_H
#define GAME_PRESUMPTION_H

#include <iostream>
#include "SDL_lib.h"
// #include "game.h"
#include "game_mapobjects.h"


/** Předvídání plamene bomby.
 * Dynamický objekt vytvářený bombou.
 * Upozorňuje na skutečnost,
 * že se v blízké době objeví na políčku plamen.
 */
class Presumption: public DynamicMO {
	public:
		/// Vytvoří plamen.
		Presumption(Animation * anim, Uint16 x, Uint16 y,
				Uint16 to_flame);
		/// Pohyb.
		virtual bool move();
		/// Vykreslení.
		virtual void draw(SDL_Surface *window, const SDL_Rect & rect);
		/// Posun animace.
		virtual void update();
		/// Typ objektu je presumpce.
		virtual OBJECT_TYPES type() const { return PRESUMPTION; }
		/// Destructor.
		virtual ~Presumption() {};
		/// Počet period do výbuchu.
		Uint16 periods_to_flame() const
			{ return periods_to_flame_; }
	protected:
		/// Animace.
		Animation * anim_;
		/// Počet period do výbuchu.
		Uint16 periods_to_flame_;
};

#endif

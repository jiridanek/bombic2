/** @file game_bonus.h
 * Herní bonus.
 * Game_bonus.h obsahuje třídu obstarávající bonusy.
 */
#ifndef GAME_BONUS_H
#define GAME_BONUS_H

#include <iostream>
#include "SDL_lib.h"
#include "game.h"
#include "game_mapobjects.h"


/** Bonus.
 * Dynamický objekt. Hoří, nechá se sbírat bombičem.
 * Sám si hlídá kdy má být viditelný.
 */
class Bonus: public DynamicMO{
	public:
		Bonus(const Surface & sur, Uint16 x, Uint16 y);
		/// Pohyb.
		virtual void move();
		/// Vykreslení.
		virtual void draw(SDL_Surface *window);
		/// Typ objektu je bonus.
		virtual OBJECT_TYPES type(){ return BONUS; }
	private:
		Surface sur_;
		bool visible_;
};

#endif

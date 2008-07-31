/** @file game_bonus.h
 * Herní bonus.
 * Game_bonus.h obsahuje třídu obstarávající bonusy.
 */
#ifndef GAME_BONUS_
#define GAME_BONUS_

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
		void move();
		/// Vykreslení.
		void draw(SDL_Surface *window);

		OBJECT_TYPES type();
	private:
		Surface sur_;
		Uint16 x_, y_;
		bool visible_;
};

#endif

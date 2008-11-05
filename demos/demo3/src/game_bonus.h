/** @file game_bonus.h
 * Herní bonus.
 * Game_bonus.h obsahuje třídu obstarávající bonusy.
 */
#ifndef GAME_BONUS_H
#define GAME_BONUS_H

#include <iostream>
#include <string>
#include "SDL_lib.h"
#include "game.h"
#include "game_mapobjects.h"


/** Bonus.
 * Dynamický objekt. Hoří, nechá se sbírat bombičem.
 * Sám si hlídá kdy má být viditelný.
 */
class Bonus: public DynamicMO{
	public:
		/// Vytvoří bonus.
		Bonus(const std::string & bonus_name,
				const Animation & anim, Uint16 x, Uint16 y);
		/// Okopíruje bonus.
		Bonus(const Bonus & bonus, Uint16 x, Uint16 y);
		/// Obnova stavu.
		virtual bool move();
		/// Vykreslení.
		virtual void draw(SDL_Surface *window);
		/// Posun animace.
		virtual void update();
		/// Typ objektu je bonus.
		virtual OBJECT_TYPES type() const { return BONUS; }
		/// Destructor
		virtual ~Bonus() {};
	private:
		/// Animace.
		Animation anim_;
		/// Je-li vidět.
		bool visible_;
		/// Jméno bonusu.
		std::string bonus_name_;
};

#endif

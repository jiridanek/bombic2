/** @file game_creature.h
 * Nestvůra.
 * Game_creature.h obsahuje třídu obstarávající nestvůry.
 */
#ifndef GAME_CREATURE_H
#define GAME_CREATURE_H

#include <iostream>
#include "SDL_lib.h"
#include "game.h"
#include "game_mapobjects.h"


/** Nestvůra.
 * Dynamický objekt, který zabíjí, umírá, chodí nebo létá.
 * Disponuje umělou inteligencí.
 */
class Creature: public DynamicMO {
	public:
		/// Vytvoří nestvůru.
		Creature(const Surface & sur_left, const Surface & sur_left_s,
			const Surface & sur_up, const Surface & sur_up_s,
			const Surface & sur_right, const Surface & sur_right_s,
			const Surface & sur_down, const Surface & sur_down_s,
			const Surface & sur_burned, Uint16 x, Uint16 y,
			Uint8 speed, Uint8 lives, Uint8 ai);
		/// Pohyb.
		virtual void move();
		/// Vykreslení.
		virtual void draw(SDL_Surface *window);
		/// Typ objektu je nestvůra.
		virtual OBJECT_TYPES type(){ return CREATURE; }
		/// Destructor
		virtual ~Creature() {};
	private:
		/// Surface pro různé stavy.
		Surface sur_left_, sur_left_s_, sur_up_, sur_up_s_,
			sur_right_, sur_right_s_, sur_down_, sur_down_s_, sur_burned_;
		/// Současné otočení.
		DIRECTION d_;
		/// Rychlost, počet životů, inteligence.
		Uint8 speed_, lives_, ai_;
};

#endif

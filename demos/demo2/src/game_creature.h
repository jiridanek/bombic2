/** @file game_creature.h
 * TODO
 * Herní Creature.
 * Game_creature.h obsahuje třídu obstarávající Creaturey.
 */
#ifndef GAME_CREATURE_
#define GAME_CREATURE_

#include <iostream>
#include "SDL_lib.h"
#include "game.h"
#include "game_mapobjects.h"


/** Nestvůra.
 * Dynamický objekt, který zabíjí, umírá, chodí nebo létá.
 * Disponuje umělou inteligencí.
 */
class Creature: public DynamicMO{
	public:
		Creature(const Surface & sur_left, const Surface & sur_left_s,
			const Surface & sur_up, const Surface & sur_up_s,
			const Surface & sur_right, const Surface & sur_right_s,
			const Surface & sur_down, const Surface & sur_down_s,
			const Surface & sur_burned, Uint16 x, Uint16 y,
			Uint8 speed, Uint8 lives, Uint8 ai);
		/// Pohyb.
		void move();
		/// Vykreslení.
		void draw(SDL_Surface *window);

		OBJECT_TYPES type();
	private:
		Surface sur_left_, sur_left_s_, sur_up_, sur_up_s_,
			sur_right_, sur_right_s_, sur_down_, sur_down_s_, sur_burned_;
		/// Současné otočení.
		DIRECTION d_;
		/// Rychlost, počet životů, inteligence.
		Uint8 speed_, lives_, ai_;
};

#endif

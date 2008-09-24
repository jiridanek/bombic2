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

class AI; // forward declaration


/** Nestvůra.
 * Dynamický objekt, který zabíjí, umírá, chodí nebo létá.
 * Disponuje umělou inteligencí.
 */
class Creature: public DynamicMO {
	friend class AI;
	friend class AI_fromKeyboard;
	public:
		/// Vytvoří nestvůru.
		Creature(const Animation & anim_up, const Animation & anim_right,
			const Animation & anim_down, const Animation & anim_left,
			const Animation & anim_burned, Uint16 x, Uint16 y,
			Uint16 speed, Uint16 lives, Uint16 ai);
		/// Pohyb.
		virtual void move();
		/// Vykreslení.
		virtual void draw(SDL_Surface *window);
		/// Typ objektu je nestvůra.
		virtual OBJECT_TYPES type(){ return CREATURE; }
		/// Destructor
		virtual ~Creature();
	protected:
		/// Animace pro daný směr.
		Animation & anim_(DIRECTION d_);
		/// Přejde na další fázi umírání.
		void die();
		/// Surface pro různé stavy.
		Animation anim_up_, anim_right_, anim_down_, anim_left_, anim_burned_;
		/// Současné otočení.
		DIRECTION d_;
		/// Umělá inteligence.
		AI* ai_;
		/// Hýbal se.
		bool moved_;
		/// Počítadlo přístupů.
		Uint32 access_counter_;
		/// Počet životů, rychlost.
		Uint16 lives_, speed_diff_, speed_rate_;
};

#endif

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
			Uint16 speed, Uint16 lives, Sint16 ai);
		/// Vytvoří nestvůru.
		Creature(const Creature & creature, Uint16 x, Uint16 y);
		/// Pohyb.
		virtual bool move();
		/// Vykreslení.
		virtual void draw(SDL_Surface *window);
		/// Posun animace.
		virtual void update();
		/// Typ objektu je nestvůra.
		virtual OBJECT_TYPES type() const { return CREATURE; }
		/// Destructor
		virtual ~Creature();
	protected:
		/// Animace pro daný směr.
		Animation & anim_(DIRECTION d_);
		/// Přejde na další fázi umírání.
		bool die();

		Animation anim_up_, ///< Animace pro pohyb vzhůru.
			anim_right_, ///< Animace pro pohyb vpravo.
			anim_down_, ///< Animace pro pohyb dolů.
			anim_left_, ///< Animace pro pohyb vlevo.
			anim_burned_; ///< Animace pro umírání.
		/// Současné otočení.
		DIRECTION d_;
		/// Umělá inteligence.
		AI* ai_;
		/// Hýbal se.
		bool moved_;
		/// Počítadlo přístupů.
		Uint32 access_counter_;
		/// Počet životů.
		Uint16 lives_,
			speed_diff_, ///< Velikost parciálního pohybu.
			speed_rate_; ///< Míra parciálního pohybu.
};

#endif

/** @file game_player.h
 * Postava bombiče.
 * Game_player.h obsahuje třídu obstarávající objekt hráče bombiče.
 */
#ifndef GAME_PLAYER_H
#define GAME_PLAYER_H

#include <iostream>
#include "SDL_lib.h"
#include "game.h"
#include "game_mapobjects.h"
#include "game_creature.h"

class AI; // forward declaration

/** Postava Bombiče.
 * Dynamický objekt, který vytváří bomby,
 * zabíjí, umírá, sbírá bonusy, chodí.
 */
class Player: public Creature {
	friend class AI_fromKeyboard;
	public:
		/// Vytvoří bombiče.
		Player(const Animation & anim_up, const Animation & anim_right,
			const Animation & anim_down, const Animation & anim_left,
			const Animation & anim_burned, Uint16 x, Uint16 y,
			Uint16 speed, Uint16 lives);
		/// Pohyb.
		virtual void move();
		/// Typ objektu je bombic.
		virtual OBJECT_TYPES type(){ return PLAYER; }
		/// Destructor
		virtual ~Player();
	private:
		/// Klávesy.
		SDLKey key_up_, key_right_, key_down_, key_left_,
			key_plant_, key_timer_;
};

#endif

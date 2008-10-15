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
			Uint16 speed, Uint16 lives, Uint16 num);
		/// Okopíruje hráče.
		Player(const Player & player, Uint16 x, Uint16 y);

		/// Pohyb.
		virtual bool move();
		/// Typ objektu je bombic.
		virtual OBJECT_TYPES type() const { return PLAYER; }
		/// Destructor
		virtual ~Player();

		/// Číslo hráče.
		Uint16 player_num() const { return num_; }
	private:
		/// Číslo hráče.
		Uint16 num_;

		SDLKey key_up_, ///< Klávesa nahoru.
		 key_right_, ///< Klávesa vpravo.
		 key_down_, ///< Klávesa dolů.
		 key_left_, ///< Klávesa vlevo.
		 key_plant_, ///< Klávesa pokládání bomby.
		 key_timer_; ///< Klávesa odpalování (pokud má časovač).
};

#endif

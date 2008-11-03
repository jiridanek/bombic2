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

/// Doba mezi odpaly.
#define TIMER_PERIOD 300

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
		/// Posun animace.
		virtual void update();
		/// Vykreslení panelu
		void drawPanel(SDL_Surface *window);
		/// Typ objektu je bombic.
		virtual OBJECT_TYPES type() const { return PLAYER; }

		virtual ~Player() {};

		/// Číslo hráče.
		Uint16 player_num() const { return num_; }
	private:
		Uint16 num_, ///< Číslo hráče.
			flamesize_, ///< Velikost plamene.
			bombs_, ///< Počet bomb.
			next_timer_; ///< Doba do dalšího odpalu.
};

#endif

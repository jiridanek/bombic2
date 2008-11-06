/** @file game_player.h
 * Postava bombiče.
 * Game_player.h obsahuje třídu obstarávající objekt hráče bombiče.
 */
#ifndef GAME_PLAYER_H
#define GAME_PLAYER_H

#include <iostream>
#include <list>
#include "SDL_lib.h"
// #include "game.h"
#include "game_mapobjects.h"
#include "game_creature.h"

/// Doba mezi odpaly.
#define TIMER_PERIOD 300

class BonusApplication;

/** Postava Bombiče.
 * Dynamický objekt, který vytváří bomby,
 * zabíjí, umírá, sbírá bonusy, chodí.
 */
class Player: public Creature {
	friend class AI_fromKeyboard;
	friend class BonusApplication;
	friend class BonusFlame;
	friend class BonusBomb;
	friend class BonusMegabomb;
	friend class BonusKicker;
	friend class BonusSlider;
	friend class BonusTimer;
	friend class BonusShield;
	friend class BonusFireman;
	friend class BonusSpeed;
	friend class BonusLive;


	public:
		/// Vytvoří bombiče.
		Player(const Animation & anim_up, const Animation & anim_right,
			const Animation & anim_down, const Animation & anim_left,
			const Animation & anim_burned, Uint16 x, Uint16 y,
			Uint16 speed, Uint16 lives, Uint16 num);
		/// Okopíruje hráče.
		Player(const Player & player, Uint16 x, Uint16 y);
		/// Zjistí, zda je hráč na stejné pozici.
		bool operator==(const Player & player) const;

		/// Pohyb.
		virtual bool move();
		/// Posun animace.
		virtual void update();
		/// Vykreslení panelu
		void draw_panel(SDL_Surface *window);
		/// Typ objektu je bombic.
		virtual OBJECT_TYPES type() const { return PLAYER; }

		virtual ~Player();

		/// Číslo hráče.
		Uint16 player_num() const
				{ return num_; }
		/// Souřadnice X.
		Uint16 getX() const
				{ return x_; }
		/// Souřadnice Y.
		Uint16 getY() const
				{ return y_; }
	private:
		Uint16 num_, ///< Číslo hráče.
			flamesize_, ///< Velikost plamene.
			bombs_, ///< Počet bomb.
			megabombs_, ///< Počet megabomb.
			next_timer_; ///< Doba do dalšího odpalu.
		bool bonus_kicker_, ///< Má kopání.
			bonus_slider_, ///< Má posílání.
			bonus_timer_, ///< Má ruční odpalování bomb.
			bonus_fireman_; ///< Má firemana.

		typedef std::list< BonusApplication * > bonuses_t;
		bonuses_t bonuses_;
};

#endif

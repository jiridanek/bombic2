/** @file game_player.h
 * Postava bombiče.
 * Game_player.h obsahuje třídu obstarávající objekt hráče bombiče.
 */
#ifndef GAME_PLAYER_H
#define GAME_PLAYER_H

#include <list>
#include <helpers/sdl.h>
#include "creature.h"

/// Doba mezi odpaly.
#define TIMER_PERIOD 300
/// Maximaální velikost plamene.
#define FLAMESIZE_MAX Uint16(10)

class BonusApplication;

/** Konfigurace hráče.
 * Konfigurace obsahuje počet životů
 * a počet sesbíraných persistentních bonusů.
 */
typedef struct {
	Uint16 lives, ///< Počet životů.
		bombs, ///< Počet bomb.
		flames, ///< Velikost plamene.
		boots; ///< Počet botiček.
	} PlayerProperties;

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
	friend class BonusIllness;
	friend class BonusIllnessConfused;
	friend class BonusIllnessStop;
	friend class BonusIllnessSlow;
	friend class BonusIllnessFast;

	public:
		/// Vytvoří bombiče.
		Player(const Animation & anim_up, const Animation & anim_right,
			const Animation & anim_down, const Animation & anim_left,
			const Animation & anim_burned, Uint16 x, Uint16 y,
			Uint16 speed, Uint16 num);
		/// Okopíruje hráče.
		Player(const Player & player, Uint16 x, Uint16 y);
		/// Zjistí, zda je hráč na stejné pozici.
		bool operator==(const Player & player) const;

		/// Pohyb.
		virtual bool move();
		/// Posun animace.
		virtual void update();
		/// Vykreslení panelu.
		void draw_panel(SDL_Surface *window, const SDL_Rect & rect);
		/// Nastaví vlastnosti.
		void set_properties(const PlayerProperties & prop);
		/// Zjistí vlastnosti.
		void get_properties(PlayerProperties & prop);

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
			boots_, ///< Počet sesbíraných botiček.
			next_timer_; ///< Doba do dalšího odpalu.
		bool bonus_kicker_, ///< Má kopání.
			bonus_slider_, ///< Má posílání.
			bonus_timer_, ///< Má ruční odpalování bomb.
			bonus_fireman_; ///< Má firemana.

		typedef std::list< BonusApplication * > bonuses_t;
		bonuses_t bonuses_;
};

#endif

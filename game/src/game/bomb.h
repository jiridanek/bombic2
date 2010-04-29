/** @file bomb.h
 * Bomba.
 * Obsahuje třídy @c Bomb a @c MegaBomb obstarávající bomby.
 */
#ifndef GAME_BOMB_H
#define GAME_BOMB_H

#include <helpers/sdl.h>
#include "mapobjects.h"
#include "presumption.h"

/** Bomba.
 * Dynamický objekt vytvářený především hráčem.
 * Vytváří plamen, může se hýbat.
 */
class Bomb: public DynamicMO {
	public:
		/// Vytvoří bombu.
		Bomb(const Animation & anim, Uint16 x, Uint16 y,
				Uint16 flamesize, Uint16 speed, bool timer);
		/// Pohyb.
		virtual bool move();
		/// Vykreslení.
		virtual void draw(SDL_Surface *window, const SDL_Rect & rect);
		/// Posun animace.
		virtual void update();
		/// Typ objektu je bomba.
		virtual OBJECT_TYPES type() const
			{ return d_==BURNED ? BOMB_STAYING : BOMB_MOVING; }
		/// Destructor
		virtual ~Bomb();

		/// Ne, není megabomba.
		virtual bool is_mega() const
			{ return false; }
		/// Přinutí bombu k explozi.
		virtual void explode();
		/// Kopne do bomby.
		void kick(DIRECTION d);
		/// Odstranění ručního odpalování.
		void remove_timer();
	protected:
		/// Animace.
		Animation anim_;

		bool explodes_, ///< Exploduje-li bomba.
			timer_; ///< Odpaluje se ručně.
		Uint16
			periods_to_flame_, ///< Počet period do výbuchu.
			access_counter_, ///< Počítadlo přístupů.
			flamesize_, ///< Velikost plamene.
			speed_diff_, ///< Velikost parciálního pohybu.
			speed_rate_; ///< Míra parciálního pohybu.
		/// Směr pohybu.
		DIRECTION d_;
		/// Seznam presumpcí.
		std::vector< Presumption* > presumptions_;
		/// Cílové políčko.
		field_t target_field_;

		/// Obnovit počet period do výbuchu.
		void update_periods_to_flame_();
		/// Pohnout bombou ve směru pohybu.
		bool move_by_direction_();
		/// Vytvořit presumpce.
		virtual void update_presumptions_();
		/// Vytvoří presumpce v jednom směru.
		void update_presumptions_from_field_(
			const field_t & start_field,
			Sint16 factor_x, Sint16 factor_y);

		/// Najít cílové políčko bomby.
		field_t find_target_field_() const;
		/// Spočítat vzdálenost dojezdu.
		Uint16 count_distance_(Uint16 periods_to_end) const;
		/// Posunout políčko.
		void move_field_by_direction_(
			field_t & field, Sint16 delta) const;
		/// Je zde nebezpečná presumpce?.
		bool is_danger_presumption_(
			const field_t & field, Uint16 periods_to_end) const;
		/// Převezme cíl od předcházející bomby.
		field_t take_target_from_bomb_before_(
			Bomb * bomb_before) const;
		/// Přidat či obnovit presumpci.
		bool update_presumption_(const field_t & field);
		/// Odstranit presumpce.
		void remove_presumptions_();
};

class MegaBomb: public Bomb {
	public:
		MegaBomb(const Animation & anim, Uint16 x, Uint16 y,
				Uint16 flamesize, Uint16 speed, bool timer);
		/// Ano, je megabomba.
		virtual bool is_mega() const
			{ return true; }
		/// Přinutí bombu k explozi.
		virtual void explode();
	protected:
		/// Vytvořit presumpce.
		virtual void update_presumptions_();
		/// Pokud může, vloží na políčko novou bombu.
		void try_insert_bomb_(const field_t & field);
};

#endif

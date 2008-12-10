/** @file game_bomb.h
 * Bomba.
 * Game_bomb.h obsahuje třídy obstarávající bomby.
 */
#ifndef GAME_BOMB_H
#define GAME_BOMB_H

#include "SDL_lib.h"
#include "game_mapobjects.h"
#include "game_presumption.h"

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
			access_counter_, ///< Počítadlo přístupů.
			flamesize_, ///< Velikost plamene.
			speed_diff_, ///< Velikost parciálního pohybu.
			speed_rate_; ///< Míra parciálního pohybu.
		/// Směr pohybu.
		DIRECTION d_;
		/// Seznam presumpcí.
		std::vector< Presumption* > presumptions_;
		/// Vytvořit presumpce.
		virtual void create_presumptions_();
		/// Najít cílové políčko bomby.
		void find_target_(Uint16 & x, Uint16 & y) const;
		/// Spočítat vzdálenost dojezdu.
		Uint16 count_distance_(Uint16 to_end) const;
		/// Přidat presumpci.
		bool add_presumption_(Uint16 x, Uint16 y);
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
		virtual void create_presumptions_();
};

#endif

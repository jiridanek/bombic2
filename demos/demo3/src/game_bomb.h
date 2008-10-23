/** @file game_bomb.h
 * Bomba.
 * Game_bomb.h obsahuje třídy obstarávající bomby.
 */
#ifndef GAME_BOMB_H
#define GAME_BOMB_H

#include <iostream>
#include "SDL_lib.h"
#include "game.h"
#include "game_mapobjects.h"

/** Bomba.
 * Dynamický objekt vytvářený především hráčem.
 * Vytváří plamen, může se hýbat.
 */
class Bomb: public DynamicMO {
	public:
		/// Vytvoří bombu.
		Bomb(const Animation & anim, Uint16 x, Uint16 y, Uint16 flamesize);
		/// Pohyb.
		virtual bool move();
		/// Vykreslení.
		virtual void draw(SDL_Surface *window);
		/// Posun animace.
		virtual void update();
		/// Typ objektu je bomba.
		virtual OBJECT_TYPES type() const { return BOMB; }
		/// Destructor
		virtual ~Bomb() {};

		/// Přinutí bombu k explozi.
		void explode();
	protected:
		/// Animace.
		Animation anim_;
		/// Exploduje-li bomba.
		bool explodes_;
		/// Velikost plamene.
		Uint16 flamesize_;
		/// Seznam presumpcí.
		std::vector< Presumption* > presumptions_;
		/// Vytvořit presumpce.
		void create_presumptions_();
		/// Posunout presumpce.
		void move_presumptions_();
		/// Odstranit presumpce.
		void remove_presumptions_();
};

#endif

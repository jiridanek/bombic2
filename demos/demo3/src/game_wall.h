/** @file game_wall.h
 * Zeď.
 * Game_wall.h obsahuje třídu obstarávající objekty hry typu zeď.
 */
#ifndef GAME_WALL_H
#define GAME_WALL_H

#include <iostream>
#include "SDL_lib.h"
#include "game.h"
#include "game_mapobjects.h"

/** Zeď.
 * Statický objekt, který neshoří, a nejde přes něj chodit,
 * většinou ani létat.
 * Plamen se o zeď zarazí.
 */
class Wall: public StaticMO{
	public:
		/// Vytvoří zeď.
		Wall(const Animation & anim, Uint16 x, Uint16 y);
		/// Okopíruje zeď.
		Wall(const Wall & wall, Uint16 x, Uint16 y, Uint16 height);
		/// Vykreslení.
		virtual void draw(SDL_Surface *window);
		/// Posun animace.
		virtual void update();
		/// Typ objektu je zeď.
		virtual OBJECT_TYPES type() const { return WALL; }
		/// Destructor
		virtual ~Wall() {};
	private:
		/// Animace zdi.
		Animation anim_;
};

#endif

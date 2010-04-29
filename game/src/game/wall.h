/** @file wall.h
 * Zeď.
 * Obsahuje třídu @c Wall obstarávající objekty hry typu zeď.
 */
#ifndef GAME_WALL_H
#define GAME_WALL_H

#include <helpers/sdl.h>
#include "mapobjects.h"

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
		virtual void draw(SDL_Surface *window, const SDL_Rect & rect);
		/// Posun animace.
		virtual void update();
		/// Typ objektu je zeď.
		virtual OBJECT_TYPES type() const { return WALL; }

		virtual ~Wall() {};
	private:
		/// Animace zdi.
		Animation anim_;
};

#endif

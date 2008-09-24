/** @file game_wall.h
 * Zed.
 * Game_wall.h obsahuje třídu obstarávající objekty hry typu zed
 */
#ifndef GAME_WALL_H
#define GAME_WALL_H

#include <iostream>
#include "SDL_lib.h"
#include "game.h"
#include "game_mapobjects.h"

/** Zed.
 * Statický objekt, který neshoří, a nejde přes něj chodit,
 * většinou ani létat.
 * Plamen se o zed zarazí.
 */
class Wall: public StaticMO{
	public:
		/// Vytvoří zeď.
		Wall(const Animation & anim, Uint16 x, Uint16 y);
		/// Vykreslí zeď.
		virtual void draw(SDL_Surface * window);
		/// Typ objektu je zeď.
		virtual OBJECT_TYPES type(){ return WALL; }
		/// Destructor
		virtual ~Wall() {};
	private:
		/// Animace zdi.
		Animation anim_;
};

#endif

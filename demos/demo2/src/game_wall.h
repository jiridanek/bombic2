/** @file game_wall.h
 * Zed.
 * Game_wall.h obsahuje třídu obstarávající objekty hry typu zed
 */
#ifndef GAME_WALL_
#define GAME_WALL_

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
		Wall(const Surface & sur, Uint16 x, Uint16 y);
		void draw(SDL_Surface * window);
		OBJECT_TYPES type();
	private:
		Surface sur_;
};

#endif

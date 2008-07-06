/** @file game_background.h
 * Pozadí mapy.
 * Game_background.h obsahuje třídu obstarávající pozadí mapy.
 */
#ifndef GAME_BACKGROUND_
#define GAME_BACKGROUND_

#include <iostream>
#include "SDL_lib.h"
#include "game.h"

/** Pozadí mapy.
 * Statický objekt jako podklad hrací plochy.
 * Lze přes něj chodit, létat, letí přes něj plamen.
 * Po výbuchu bomby může změnit vzhled.
 */
class Background: public StaticMO{
	public:
		/// Vytvoří pozadí jednoho políčka.
		Background(const Surface & sur_clean,const Surface & sur_burned,
				Uint16 x, Uint16 y);
		/// Vykreslení.
		void draw(SDL_Surface * window);
		std::string type();
	private:
		Surface sur_clean_, sur_burned_;
		Uint16 x_, y_;
};

#endif

/** @file game_background.h
 * Pozadí mapy.
 * Game_background.h obsahuje třídu obstarávající pozadí mapy.
 */
#ifndef GAME_BACKGROUND_H
#define GAME_BACKGROUND_H

#include <iostream>
#include "SDL_lib.h"
#include "game.h"
#include "game_mapobjects.h"

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
		virtual void draw(SDL_Surface * window);
		/// Typ objektu je pozadí.
		virtual OBJECT_TYPES type(){ return BACKGROUND; }
		/// Destructor
		virtual ~Background() {};
	private:
		Surface sur_clean_, sur_burned_;
};


#endif

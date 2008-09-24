/** @file game_box.h
 * Bedna.
 * Game_box.h obsahuje třídu obstarávající bednu.
 */
#ifndef GAME_BOX_H
#define GAME_BOX_H

#include <iostream>
#include "SDL_lib.h"
#include "game.h"
#include "game_mapobjects.h"

/** Bedna.
 * Dynamický objekt, vytvořený na začátku hry.
 * Při zasažení plamenem shoří,
 * plamen přes něj neletí, nelze přes něj chodit ani létat.
 * Malá změna, bedna nevytváří bonus.
 */
class Box: public DynamicMO{
	public:
		Box(const Animation & anim, const Animation & anim_burning,
			Uint16 toplapping, Uint16 x, Uint16 y);
		/// Pohyb.
		virtual void move();
		/// Vykreslení.
		virtual void draw(SDL_Surface *window);
		/// Typ objektu je bedna.
		virtual OBJECT_TYPES type(){ return BOX; }
		/// Destructor
		virtual ~Box() {};
	private:
		Animation anim_, anim_burning_;
		Uint16 toplapping_, height_, width_;
};

#endif

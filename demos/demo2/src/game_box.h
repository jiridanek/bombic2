/** @file game_box.h
 * Bedna.
 * Game_box.h obsahuje třídu obstarávající bednu.
 */
#ifndef GAME_BOX_
#define GAME_BOX_

#include <iostream>
#include "SDL_lib.h"
#include "game.h"

/** Bedna.
 * Dynamický objekt, vytvořený na začátku hry.
 * Při zasažení plamenem shoří,
 * plamen přes něj neletí, nelze přes něj chodit ani létat.
 * Malá změna, bedna nevytváří bonus.
 */
class Box: public DynamicMO{
	public:
		Box(const Surface & sur_img, const Surface & sur_burning,
			Uint16 toplapping, Uint16 x, Uint16 y);
		/// Pohyb.
		void move();
		/// Vykreslení.
		void draw(SDL_Surface *window);

		OBJECT_TYPES type();
	private:
		Surface sur_img_, sur_burning_;
		Uint16 x_, y_, toplapping_, height_, width_;
};

#endif

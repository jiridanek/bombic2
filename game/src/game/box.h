/** @file game_box.h
 * Bedna.
 * Game_box.h obsahuje třídu obstarávající bednu.
 */
#ifndef GAME_BOX_H
#define GAME_BOX_H

#include <helpers/sdl.h>
#include "mapobjects.h"

/** Bedna.
 * Dynamický objekt, vytvořený na začátku hry.
 * Při zasažení plamenem shoří,
 * plamen přes něj neletí, nelze přes něj chodit ani létat.
 * Malá změna, bedna nevytváří bonus.
 */
class Box: public DynamicMO{
	public:
		/// Vytvoří bednu.
		Box(const Animation & anim, const Animation & anim_burning,
			Uint16 toplapping, Uint16 x, Uint16 y);
		/// Okopíruje bednu.
		Box(const Box & box, Uint16 x, Uint16 y);
		/// Zetová souřadnice.
		virtual Uint16 getZ() const;
		/// Zjištění, zda nemá shořet.
		virtual bool move();
		/// Vykreslení.
		virtual void draw(SDL_Surface *window, const SDL_Rect & rect);
		/// Posun animace.
		virtual void update();
		/// Typ objektu je bedna.
		virtual OBJECT_TYPES type() const { return BOX; }
		/// Destructor
		virtual ~Box() {};
	private:
		/// Animace, animace hoření.
		Animation anim_, anim_burning_;
		bool burning_, ///< Hoří.
			burned_; /// Už shořel.

		Uint16 toplapping_, ///< Horní přesah.
			height_, ///< Výška v políčkách.
			width_; ///< Šířka v políčkách.
};

#endif

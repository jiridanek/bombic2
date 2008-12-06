/** @file game_floorobject.h
 * Objekt na zemi.
 * Game_floorobject.h obsahuje třídu obstarávající objekty na zemi.
 */
#ifndef GAME_FLOOROBJECT_H
#define GAME_FLOOROBJECT_H

#include "SDL_lib.h"
#include "game_mapobjects.h"

/** Objekt ležící na zemi.
 * Statický objekt sloužící jako lokální změna pozadí mapy.
 * Nehoří, jde přes něj chodit i létat, letí přes něj plamen.
 */
class Floorobject: public StaticMO{
	public:
		/// Vytvoří objekt.
		Floorobject(const Animation & anim, Uint16 x, Uint16 y);
		/// Okopíruje objekt.
		Floorobject(const Floorobject & floorobject, Uint16 x, Uint16 y);
		/// Vykreslení.
		virtual void draw(SDL_Surface *window, const SDL_Rect & rect);
		/// Posun animace.
		virtual void update();
		/// Typ objekt na zemi.
		virtual OBJECT_TYPES type() const { return FLOOROBJECT; }
		/// Destructor
		virtual ~Floorobject() {};
	private:
		/// Animace objektu.
		Animation anim_;
};

#endif

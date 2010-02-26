/** @file game_background.h
 * Pozadí mapy.
 * Game_background.h obsahuje třídu obstarávající pozadí mapy.
 */
#ifndef GAME_BACKGROUND_H
#define GAME_BACKGROUND_H

#include <helpers/sdl.h>
#include "mapobjects.h"

/** Pozadí mapy.
 * Statický objekt jako podklad hrací plochy.
 * Lze přes něj chodit, létat, letí přes něj plamen.
 * Po výbuchu bomby může změnit vzhled.
 */
class Background: public StaticMO{
	public:
		/// Vytvoří pozadí jednoho políčka.
		Background(const Animation & anim_clean,const Animation & anim_burned,
				Uint16 x, Uint16 y);
		/// Okopíruje pozadí jednoho políčka.
		Background(const Background & background, Uint16 x, Uint16 y);
		/// Vykreslení.
		virtual void draw(SDL_Surface *window, const SDL_Rect & rect);
		/// Posun animace.
		virtual void update();
		/// Spálení.
		void set_burned() { burned_ = true; }
		/// Typ objektu je pozadí.
		virtual OBJECT_TYPES type() const { return BACKGROUND; }
		/// Destructor
		virtual ~Background() {};
	private:
		/// Animace před výbuchem.
		Animation anim_clean_,
		/// Animace po výbuchu.
			anim_burned_;
		/// Je spálený.
		bool burned_;
};


#endif

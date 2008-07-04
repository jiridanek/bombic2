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
















/*****************************************************************/

/** Postava Bombiče.
 * Dynamický objekt, který vytváří bomby,
 * zabíjí, umírá, sbírá bonusy, chodí.
 */
class Bomber: public DynamicMO{
	public:
		/// Pohyb.
		void move();
		/// Vykreslení.
		void draw();
};

/** Nestvůra.
 * Dynamický objekt, který zabíjí, umírá, chodí nebo létá.
 * Disponuje umělou inteligencí.
 */
class Creature: public DynamicMO{
	public:
		/// Pohyb.
		void move();
		/// Vykreslení.
		void draw();
};

/** Bedna.
 * Dynamický objekt, vytvořený na začátku hry.
 * Při zasažení plamenem shoří, vytváří bonus,
 * plamen přes něj neletí, nelze přes něj chodit ani létat.
 */
class Box: public DynamicMO{
	public:
		/// Pohyb.
		void move();
		/// Vykreslení.
		void draw();
};

/** Bomba.
 * Dynamický objekt vytvářený především bombičem.
 * Vytváří plamen, může se hýbat.
 */
class Bomb: public DynamicMO{
	public:
		/// Pohyb.
		void move();
		/// Vykreslení.
		void draw();
};

/** Plamen bomby.
 * Dynamický objekt vytvářený především bombou.
 * Zabíjí, sbírá (spaluje) bonusy, nehýbe se.
 */
class Flame: public DynamicMO{
	public:
		/// Pohyb.
		void move();
		/// Vykreslení.
		void draw();
};

/** Bonus.
 * Dynamický objekt. Hoří, nechá se sbírat bombičem.
 */
class Bonus: public DynamicMO{
	public:
		/// Pohyb.
		void move();
		/// Vykreslení.
		void draw();
};

#endif

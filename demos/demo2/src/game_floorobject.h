/** @file game_floorobject.h
 * Objekt na zemi.
 * Game_floorobject.h obsahuje třídu obstarávající objekty na zemi.
 */
#ifndef GAME_FLOOROBJECT_H
#define GAME_FLOOROBJECT_H

#include <iostream>
#include "SDL_lib.h"
#include "game.h"
#include "game_mapobjects.h"

/** Objekt ležící na zemi.
 * Statický objekt sloužící jako lokální změna pozadí mapy.
 * Nehoří, jde přes něj chodit i létat, letí přes něj plamen.
 */
class Floorobject: public StaticMO{
	public:
		Floorobject(const Surface & sur, Uint16 x, Uint16 y);
		virtual void draw(SDL_Surface * window);
		/// Typ objekt na zemi.
		virtual OBJECT_TYPES type(){ return FLOOROBJECT; }
	private:
		Surface sur_;
};

/************************************************** /

/** Postava Bombiče.
 * Dynamický objekt, který vytváří bomby,
 * zabíjí, umírá, sbírá bonusy, chodí.
 * /
class Bomber: public DynamicMO{
	public:
		/// Pohyb.
		void move();
		/// Vykreslení.
		void draw();
};

/** Bomba.
 * Dynamický objekt vytvářený především bombičem.
 * Vytváří plamen, může se hýbat.
 * /
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
 * /
class Flame: public DynamicMO{
	public:
		/// Pohyb.
		void move();
		/// Vykreslení.
		void draw();
};
/***********************************************/
#endif

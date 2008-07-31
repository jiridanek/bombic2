/** @file game_mapobjects.h
 * Statické a dynamické objekty mapy.
 * Game_mapObjects.h obsahuje čistě virtuální třídy objektů mapy.
 * Velmi obecně implementuje, rozhraní objektů.
 * MapObject je nejobecnější představa objektu, dále jsou zde třídy DynamicMO a StaticMO.
 */
#ifndef GAME_MAPOBJECTS_
#define GAME_MAPOBJECTS_

#include <iostream>
#include "SDL_lib.h"

/// Všechny typy obejktů
enum OBJECT_TYPES { BACKGROUND, FLOOROBJECT, WALL, BOX, BONUS, CREATURE, PLAYER };
/// Čtyři směry + hoření
enum DIRECTION { UP, RIGHT, DOWN, LEFT, BURNED };

/** Obecný objekt hry.
 * Čistě virtuální třída zastřešující veškeré objekty v mapě.
 */
class MapObject{
	public:
		/// Vykreslení.
		virtual void draw(SDL_Surface *)=0;
		/// Typ objektu.
		virtual OBJECT_TYPES type()=0;
};

/** Dynamický objekt hry.
 * Objekt v mapě, který během hry vzniká, zaniká
 * nebo se v mapě pohybuje. Má také možnost animace.
 */
class DynamicMO: public MapObject{
	public:
		/// Pohyb.
		virtual void move()=0;
	protected:
		/// Nastavení správného políčka v mapě.
		void setFieldInMap(Uint16 old_x, Uint16 old_y, Uint16 x, Uint16 y);
		/// Vycentruje objekt na políčko - je-li třeba.
		void centerPosition(Uint16 & x, Uint16 & y);
};

/** Statický objekt hry.
 * Objekt v mapě, který je v mapě od začátku do konce hry bez pohybu,
 * pouze s možností animace.
 */
class StaticMO: public MapObject{
	public:
};


#endif

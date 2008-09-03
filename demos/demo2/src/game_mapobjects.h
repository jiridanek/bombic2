/** @file game_mapobjects.h
 * Statické a dynamické objekty mapy.
 * Game_mapObjects.h obsahuje čistě virtuální třídy objektů mapy.
 * Velmi obecně implementuje, rozhraní objektů.
 * MapObject je nejobecnější představa objektu, dále jsou zde třídy DynamicMO a StaticMO.
 */
#ifndef GAME_MAPOBJECTS_H
#define GAME_MAPOBJECTS_H

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
		MapObject():x_(0),y_(0) {};
		/// Nastaví souřadnice objektu.
		MapObject(Uint16 x, Uint16 y):x_(x),y_(y) {};
		/// z-ová souřadnice.
		virtual Uint16 getZ() const =0;
		/// Vykreslení.
		virtual void draw(SDL_Surface *)=0;
		/// Typ objektu.
		virtual OBJECT_TYPES type()=0;
	protected:
		/// Souřadnice polohy v pixelech.
		Uint16 x_, y_;
};

/// Porovnání obejktů mapy podle souřadnice z.
bool isUnder(const MapObject * a, const MapObject * b);

/** Dynamický objekt hry.
 * Objekt v mapě, který během hry vzniká, zaniká
 * nebo se v mapě pohybuje. Má také možnost animace.
 */
class DynamicMO: public MapObject{
	public:
		/// Nastaví souřadnice objektu.
		DynamicMO(Uint16 x, Uint16 y):MapObject(x, y) {};
		/// z-ová souřadnice.
		virtual Uint16 getZ() const;
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
		/// Nastaví souřadnice objektu.
		StaticMO(Uint16 x, Uint16 y):MapObject(x, y) {};
		/// z-ová souřadnice.
		virtual Uint16 getZ() const;
};

/** Predikát na zjištění typu objektu mapy.
 * Inicializace typem objektu nebo objektem,
 * volání operátoru kulatých závorek jako volání logického predikátu.
 */
class isTypeOf {
	public:
		/// Inicializuje predikát požadovaným typem.
		explicit isTypeOf(OBJECT_TYPES type);
		/// Inicializuje predikát požadovaným typem skrze objekt.
		explicit isTypeOf(MapObject * object);
		/// Volání predikátu.
		bool operator()(MapObject * object);
	private:
		/// Uložený typ pro srovnání.
		OBJECT_TYPES type_;
};

#endif

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

/** Všechny typy obejktů.
 * @see MapObject::type() and derived
 * @see isTypeOf
 */
enum OBJECT_TYPES { BACKGROUND, FLOOROBJECT, WALL,
	BOX, BONUS, CREATURE, PLAYER, FLAME, PRESUMPTION, BOMB_STAYING, BOMB_MOVING, NONE };
#define OBJECT_TYPES_COUNT 12

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
		/// Porovnání podle y-ové souřadnice.
		virtual bool isUpwards(const MapObject & obj) const;
		/// Vykreslení.
		virtual void draw(SDL_Surface * window, const SDL_Rect & rect)=0;
		/// Posunutí animace.
		virtual void update()=0;
		/// Typ objektu.
		virtual OBJECT_TYPES type() const =0;

		virtual ~MapObject(){};
	protected:
		/// Souřadnice polohy v pixelech.
		Uint16 x_,
		/// Souřadnice polohy v pixelech.
			y_;
};

/// Porovnání obejktů mapy podle souřadnice z (výšky pro vykreslení).
bool isUnder(const MapObject * a, const MapObject * b);
/// Porovnání obejktů mapy podle souřadnice y (výšky na obrazovce).
bool isUpwards(const MapObject * a, const MapObject * b);

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
		virtual bool move()=0;

		virtual ~DynamicMO(){};
	protected:
		/// Nastavení správného políčka v mapě.
		void setFieldInMap(Uint16 old_x, Uint16 old_y);
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

		virtual ~StaticMO(){};
};

/** Predikát na zjištění typu objektu mapy.
 * Inicializace typem objektu,
 * volání operátoru kulatých závorek jako volání logického predikátu.
 */
class isTypeOf {
	public:
		/// Inicializuje predikát požadovanými typy.
		isTypeOf(OBJECT_TYPES type1=NONE, OBJECT_TYPES type2=NONE,
				OBJECT_TYPES type3=NONE, OBJECT_TYPES type4=NONE,
				OBJECT_TYPES type5=NONE, OBJECT_TYPES type6=NONE);
		/// Přidání typu.
		isTypeOf & addType(OBJECT_TYPES type);
		/// Vyprázdnění typů.
		isTypeOf & clear();
		/// Volání predikátu.
		bool operator()(MapObject * object) const;

		static isTypeOf
			isFlame,
			isPresumption,
			isBgType,
			isAnyBomb,
			isWallBox,
			isWallBoxPlayer,
			isWallBoxBomb,
			isWallBoxAnyBomb,
			isWallBoxBombFlame,
			isWallBoxAnyBombFlame,
			isWallBoxBombFlamePresumption;


		/// TODO ruzne uzitecne druhy predikatu
	private:
		/// Uložené typy pro srovnání.
		bool types_[OBJECT_TYPES_COUNT];
};

#endif

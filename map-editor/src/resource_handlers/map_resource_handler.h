/** @file map_resource_handler.h
 * @c MapResourceHandler obstarava nacitani a ukladani mapy.
 */

#ifndef MAP_RESOURCE_HANDLER_H_GUARD_
#define MAP_RESOURCE_HANDLER_H_GUARD_

#include <QDomElement>
#include <QDomNode>

class BombicMap;
class BombicMapObject;

/** Handler vnejsich prostredku mapy.
 * Obstarava nacitani a ukladani mapy.
 */
class MapResourceHandler {

	public:
		/// Vytvori novou prazdnou mapu.
		BombicMap * createEmptyMap();
		/// Vytvori novou mapu.
		BombicMap * createMap(const QString & name);
		/// Ulozi mapu.
		bool saveMap(BombicMap * map);

	private:
		/// Nacte hrace mapy
		bool loadMapPlayers(BombicMap * map);
		bool loadMapPlayers(const QDomElement & playersEl,
				BombicMap * map);
		/// Nacte objekty na zemi v mape.
		bool loadMapFloorobjects(const QDomElement & floorsEl,
				BombicMap * map);
		/// Nacte zdi mapy.
		bool loadMapWalls(const QDomElement & wallsEl,
				BombicMap * map);
		/// Nacte bedny mapy.
		bool loadMapBoxes(const QDomElement & boxesEl,
				BombicMap * map);
		/// Nacte prisery mapy.
		bool loadMapCreatures(const QDomElement & creaturesEl,
				BombicMap * map);

		/// Nacte pole na kterych se nemaji generovat bedny.
		bool loadMapNoboxes(const QDomElement & dontGenerateEl,
				BombicMap * map);
		/// Nacte pole na kterych se nemaji generovat prisery.
		bool loadMapNocreatures(const QDomElement & dontGenerateEl,
				BombicMap * map);

		/// Vlozi do mapy objekt na urcite pozice.
		bool insertMapObjects(const QDomElement & positionEl,
				BombicMapObject * insertedObject, BombicMap * map);

		/// Vlozi do mapy objekt na urcitou (jednu) pozici.
		bool insertMapObject(const QDomElement & positionEl,
				BombicMapObject * insertedObject, BombicMap * map);
};

#endif

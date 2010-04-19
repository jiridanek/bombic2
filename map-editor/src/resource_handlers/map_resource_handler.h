/** @file map_resource_handler.h
 * @c MapResourceHandler obstarava nacitani a ukladani mapy.
 */

#ifndef MAP_RESOURCE_HANDLER_H_GUARD_
#define MAP_RESOURCE_HANDLER_H_GUARD_

#include <QDomElement>
#include <QDomNode>
#include <QHash>

#include "../bombic/map.h"
#include "../bombic/map_object.h"

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
		/// Nazev definujiciho xml elementu podle typu objektu.
		QString objectsElName(BombicMapObject::Type objectType);
		/// Nazev pozicniho xml elementu podle typu objektu.
		QString positionElName(BombicMapObject::Type objectType);

		/******** loading *********/

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

		/******* saving ***********/

		/// Seznam policek umisteni (pozic).
		typedef QList<BombicMap::Field> PositionsT;
		/// Objekt mapy pro ulozeni.
		typedef struct _MapObjectS {
			/// Konstruktor inicializuje pocet na nulu.
			_MapObjectS(): generated(0) {}
			/// Umisteni objektu.
			PositionsT positions;
			/// Pocet nahodne generovanych objektu.
			int generated;
		} MapObjectT;
		/// Objekt podle jeho jmena.
		typedef QHash<QString, MapObjectT> ObjectsByNameT;
		/// Objekty podle typu objektu.
		typedef QHash<BombicMapObject::Type, ObjectsByNameT> ObjectsByTypeT;

		/** Data mapy pro ulozeni.
		 * Tato struktura tvori mezikrok v ukladani mapy.
		 * Prvne se ziskaji data z mapy (ulozena hezky pro editaci).
		 * Ulozi se do @c MapDataT hezky pro ukladani.
		 * Potom se ulozi do xml dokumentu a nasledne do souboru.
		 */
		typedef struct {
			/// Nazev.
			QString name;
			/// Sirka.
			int width;
			/// Vyska.
			int height;
			/// Pozadi.
			QString background;
			/// Hraci mapy.
			ObjectsByNameT players;
			/// Objekty mapy.
			ObjectsByTypeT objects;
			/// Policka zakazana pro bedny.
			PositionsT noBoxes;
			/// Policka zakazana pro prisery.
			PositionsT noCreatures;
		} MapDataT;


		void initMapDataToSave(MapDataT & mapData, BombicMap * map);

		void registerPlacedObject(MapDataT & mapData,
				BombicMapObject * object,
				BombicMap::Field field);

		void countGeneratedObjects(ObjectsByTypeT & objects,
				const BombicMap::ObjectListT & generatedObjects);

		void mapDataToXml(const MapDataT & mapData,
				QDomElement & rootEl);

		void positionsToXml(const PositionsT & positions,
				QDomElement & parentEl,
				const QString & positionElTagName);
};

#endif

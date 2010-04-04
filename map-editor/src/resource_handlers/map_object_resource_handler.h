/** @file map_object.h
 * Obecny resource handler pro objekt mapy MapObjectResourceHandler
 * predstavuje jednotne api pro nacitani objektu mapy.
 */

#ifndef MAP_OBJECT_RESOURCE_HANDLER_H_GUARD_
#define MAP_OBJECT_RESOURCE_HANDLER_H_GUARD_

#include <QDomElement>
#include "bombic/map_object.h"

/** Obecny abstraktni handler pro vytvareni objektu mapy.
 * Definuje API, kterym lze vytvaret objekt mapy. 
 */
class MapObjectResourceHandler {

	public:
		/// Vytvorit novy specializovany resource handler.
		static MapObjectResourceHandler * createResourceHandler(
				const QDomElement & rootEl);
		/// Vytvorit novy objekt mapy.
		virtual BombicMapObject * createMapObject(
				const QDomElement & rootEl) = 0;
		/// Typ resource handleru.
		virtual BombicMapObject::Type type() = 0;
};

#endif

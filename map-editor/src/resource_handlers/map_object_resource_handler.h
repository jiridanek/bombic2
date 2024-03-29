/** @file map_object_resource_handler.h
 * Obecny resource handler pro objekt mapy @c MapObjectResourceHandler
 * predstavuje jednotne api pro nacitani objektu mapy.
 */

#ifndef MAP_OBJECT_RESOURCE_HANDLER_H_GUARD_
#define MAP_OBJECT_RESOURCE_HANDLER_H_GUARD_

#include <QDomElement>
#include <QLinkedList>
#include "../bombic/map_object.h"
#include "../resource_handler.h"

#include "resource_handler_functions.h"

/** Obecny abstraktni handler pro vytvareni objektu mapy.
 * Definuje API, kterym lze vytvaret objekt mapy.
 * Sam drzi instance vsech handleru specializovanych
 * na jednotlive typy objektu a pomoci sofistikovaneho mechanismu
 * vybere podle korenoveho elementu dokumentu, ktery specifikuje nejaky
 * objekt mapy, resource handler, ktery umi tento dokument zpracovat
 * a vytvorit odpovidajici objekt.
 */
class MapObjectResourceHandler {

	public:
		/// Vytvorit novy objekt mapy.
		virtual BombicMapObject * createMapObject(
				const QDomElement & rootEl) =0;
		/// Typ resource handleru.
		virtual BombicMapObject::Type type() =0;
		/// Zda umi nacist objekt reprezentovany takovym XML elementem.
		virtual bool canHandle(const QDomElement & rootEl) =0;

		/// Specializovany resource handler podle XML elementu.
		static MapObjectResourceHandler * resourceHandler(
				const QDomElement & rootEl);

		/// Inicializace dostupnych (registrovanych) handleru.
		static void initResourceHandlers();

	protected:
		/// Najde podelement podle jmena.
		inline static bool getSubElement(
				const QDomElement & el, QDomElement & subEl,
				const QString & subElTagName = "img") {
			return ResourceHandlerFunctions::getSubElement(
				el, subEl, subElTagName);
		}
		/// Precte integer hodnotu atributu.
		inline static bool getIntAttr(
				const QDomElement & el,
				int & attr, const QString & attrName,
				bool successIfMissing = false) {
			return ResourceHandlerFunctions::getIntAttr(
				el, attr, attrName, successIfMissing);
		}
		/// Precte hodnotu atributu x,y.
		inline static bool getAttrsXY(
				const QDomElement & el,
				int & x, int & y) {
			return ResourceHandlerFunctions::getAttrsXY(el, x, y);
		}
		/// Zdrojovy obrazek.
		inline static const QPixmap & sourcePixmap() {
			return RESOURCE_HANDLER->sourcePixmap_;
		}


	private:
		/// Typ seznamu handleru.
		typedef QLinkedList<MapObjectResourceHandler *> RHListT;
		/// Seznam dostupnych (registrovanych) handleru.
		static RHListT resourceHandlers_;
};

#endif

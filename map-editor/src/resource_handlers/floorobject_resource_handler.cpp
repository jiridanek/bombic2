
#include <QPixmap>

#include <constants.h>

#include "floorobject_resource_handler.h"

#include "../bombic/map_object.h"
#include "../bombic/floorobject.h"

/** @details
 * Umi zpracovavat dokument s korenovym elementem <em>floorobject</em>.
 * @param rootEl korenovy element dokumentu, ktery reprezentuje objekt mapy
 * @return Zda zpracovava takove dokumenty.
 */
bool FloorobjectResourceHandler::canHandle(const QDomElement & rootEl) {
	return rootEl.tagName()=="floorobject";
}

/** @details
 * Ze zadaneho korenoveho elementu <em>floorobject</em> vytvori prototyp
 * objektu reprezentovane timto elementem. Pokud se pri zpracovani dokumentu
 * vyskytne chyba, zobrazi chybove hlaseni.
 * Zda jde o element se spravnym nazvem se jiz nezkouma, proto
 * by melo byt vzdy drive otestovano pomoci @c canHandle().
 * Predpoklada jiz nacteny zdrojovy obrazek v @c ResourceHandler.
 * @param rootEl korenovy element reprezentujici nacitany objekt na zemi
 * @return Nove vytvoreny prototyp objektu (volajici se stava vlastnikem).
 * @retval 0 Doslo k chybe, objekt na zemi nemuze byt vytvoren.
 */
BombicMapObject * FloorobjectResourceHandler::createMapObject(
		const QDomElement & rootEl) {
	QDomElement imgEl;
	if(getSubElement(rootEl, imgEl)) {
		// get attributes
		int x = 0;
		int y = 0;
		int w = 1;
		int h = 1;
		bool success =
			getAttrsXY(imgEl, x, y) &&
			getIntAttr(imgEl, w, "width", true) &&
			getIntAttr(imgEl, h, "height", true);
		if(!success) {
			return 0;
		}
		// get pixmap
		QPixmap pixmap = sourcePixmap().copy(
			x, y, w*CELL_SIZE, h*CELL_SIZE);

		return new BombicFloorobject(
			rootEl.attribute("name"), pixmap, w, h);
	} else {
		return 0;
	}
}

/**
 * @retval BombicMapObject::Floorobject Vzdy.
 */
BombicMapObject::Type FloorobjectResourceHandler::type() {
	return BombicMapObject::Floorobject;
}

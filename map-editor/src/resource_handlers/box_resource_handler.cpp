
#include <QPixmap>

#include <constants.h>

#include "box_resource_handler.h"

#include "../bombic/map_object.h"
#include "../bombic/box.h"

/** @details
 * Umi zpracovavat dokument s korenovym elementem <em>box</em>.
 * @param rootEl korenovy element dokumentu, ktery reprezentuje objekt mapy
 * @return Zda zpracovava takove dokumenty.
 */
bool BoxResourceHandler::canHandle(const QDomElement & rootEl) {
	return rootEl.tagName()=="box";
}

/** @details
 * Ze zadaneho korenoveho elementu <em>box</em> vytvori prototyp bedny
 * reprezentovane timto elementem. Pokud se pri zpracovani dokumentu
 * vyskytne chyba, zobrazi chybove hlaseni.
 * Zda jde o element se spravnym nazvem se jiz nezkouma, proto
 * by melo byt vzdy drive otestovano pomoci @c canHandle().
 * Predpoklada jiz nacteny zdrojovy obrazek v @c ResourceHandler.
 * @param rootEl korenovy element reprezentujici nacitanou bednu
 * @return Nove vytvoreny prototyp bedny (volajici se stava vlastnikem).
 * @retval 0 Doslo k chybe, bedna nemuze byt vytvoren.
 */
BombicMapObject * BoxResourceHandler::createMapObject(
		const QDomElement & rootEl) {
	QDomElement imgEl;
	if(getSubElement(rootEl, imgEl)) {
		// get attributes
		int x = 0;
		int y = 0;
		int w = 1;
		int h = 1;
		int t = 0; // toplapping
		bool success =
			getAttrsXY(imgEl, x, y) &&
			getIntAttr(imgEl, w, "width", true) &&
			getIntAttr(imgEl, h, "height", true) &&
			getIntAttr(imgEl, t, "toplapping", true);
		if(!success) {
			return 0;
		}
		// get pixmap
		QPixmap pixmap = sourcePixmap().copy(
			x, y, w*CELL_SIZE, (h+t)*CELL_SIZE);

		return new BombicBox(
			rootEl.attribute("name"), pixmap, w, h, t);
	} else {
		return 0;
	}
}

/**
 * @retval BombicMapObject::Box Vzdy.
 */
BombicMapObject::Type BoxResourceHandler::type() {
	return BombicMapObject::Box;
}

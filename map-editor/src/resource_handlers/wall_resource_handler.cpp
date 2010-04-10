
#include <QPixmap>

#include <constants.h>

#include "wall_resource_handler.h"

#include "../bombic/map_object.h"
#include "../bombic/wall.h"

/** @details
 * Umi zpracovavat dokument s korenovym elementem <em>wall</em>.
 * @param rootEl korenovy element dokumentu, ktery reprezentuje objekt mapy
 * @return Zda zpracovava takove dokumenty.
 */
bool WallResourceHandler::canHandle(const QDomElement & rootEl) {
	return rootEl.tagName()=="wall";
}

/** @details
 * Ze zadaneho korenoveho elementu <em>wall</em> vytvori prototyp zdi
 * reprezentovane timto elementem. Pokud se pri zpracovani dokumentu
 * vyskytne chyba, zobrazi chybove hlaseni.
 * Zda jde o element se spravnym nazvem se jiz nezkouma, proto
 * by melo byt vzdy drive otestovano pomoci @c canHandle().
 * Predpoklada jiz nacteny zdrojovy obrazek v @c ResourceHandler.
 * @param rootEl korenovy element reprezentujici nacitanou zed
 * @return Nove vytvoreny prototyp zdi (volajici se stava vlastnikem).
 * @retval 0 Doslo k chybe, zed nemuze byt vytvorena.
 */
BombicMapObject * WallResourceHandler::createMapObject(
		const QDomElement & rootEl) {
	QDomElement imgEl;
	if(getSubElement(rootEl, imgEl)) {
		return createWall(rootEl.attribute("name"), imgEl);
	} else {
		return 0;
	}
}

/** @details
 * Ze zadaneho elementu vytvori prototyp zdi.
 * Element by mel odpovidat elementu <em>img</em> v definici zdi,
 * nebo elementum obvodovych zdi v definici pozadi.
 * Predpoklada jiz nacteny zdrojovy obrazek v @c ResourceHandler.
 * @param name jmeno vytvarene zdi
 * @param rootEl element obsahujici parametry zdi
 * @return Nove vytvoreny prototyp zdi (volajici se stava vlastnikem).
 * @retval 0 Doslo k chybe, zed nemuze byt vytvorena.
 */
BombicWall * WallResourceHandler::createWall(const QString & name,
		const QDomElement & imgEl) {
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

	return new BombicWall(name, pixmap, w, h, t);
}

/**
 * @retval BombicMapObject::Wall Vzdy.
 */
BombicMapObject::Type WallResourceHandler::type() {
	return BombicMapObject::Wall;
}

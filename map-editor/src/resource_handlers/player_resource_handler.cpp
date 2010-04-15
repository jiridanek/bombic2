
#include <QPixmap>

#include <constants.h>

#include "player_resource_handler.h"

#include "../bombic/map_object.h"
#include "../bombic/player.h"

/** @details
 * Umi zpracovavat dokument s korenovym elementem <em>player</em>.
 * @param rootEl korenovy element dokumentu, ktery reprezentuje objekt mapy
 * @return Zda zpracovava takove dokumenty.
 */
bool PlayerResourceHandler::canHandle(const QDomElement & rootEl) {
	return rootEl.tagName()=="player";
}

/** @details
 * Ze zadaneho korenoveho elementu <em>player</em> vytvori prototyp hrace
 * reprezentovane timto elementem. Pokud se pri zpracovani dokumentu
 * vyskytne chyba, zobrazi chybove hlaseni.
 * Zda jde o element se spravnym nazvem se jiz nezkouma, proto
 * by melo byt vzdy drive otestovano pomoci @c canHandle().
 * Predpoklada jiz nacteny zdrojovy obrazek v @c ResourceHandler.
 * @param rootEl korenovy element reprezentujici nacitaneho hrace
 * @return Nove vytvoreny prototyp hrace (volajici se stava vlastnikem).
 * @retval 0 Doslo k chybe, hrac nemuze byt vytvorena.
 */
BombicMapObject * PlayerResourceHandler::createMapObject(
		const QDomElement & rootEl) {
	QDomElement dirEl;
	if(!getSubElement(rootEl, dirEl, "down")) {
		return 0;
	}
	// get attributes
	int x = 0;
	int y = 0;
	int w = 0;
	int h = 0;
	bool success =
		getAttrsXY(dirEl, x, y) &&
		getIntAttr(rootEl, w, "width") &&
		getIntAttr(rootEl, h, "height");
	if(!success) {
		return 0;
	}
	// get pixmap
	QPixmap pixmap = sourcePixmap().copy(x, y, w, h);

	return new BombicPlayer(
		rootEl.attribute("name"), pixmap);
}

/**
 * @retval BombicMapObject::Player Vzdy.
 */
BombicMapObject::Type PlayerResourceHandler::type() {
	return BombicMapObject::Player;
}

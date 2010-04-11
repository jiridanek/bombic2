
#include <QPixmap>

#include <constants.h>

#include "creature_resource_handler.h"

#include "../bombic/map_object.h"
#include "../bombic/creature.h"

/** @details
 * Umi zpracovavat dokument s korenovym elementem <em>creature</em>
 * a definovanym atributem <em>intelligence</em> (bez inteligence je to hrac).
 * @param rootEl korenovy element dokumentu, ktery reprezentuje objekt mapy
 * @return Zda zpracovava takove dokumenty.
 */
bool CreatureResourceHandler::canHandle(const QDomElement & rootEl) {
	return rootEl.tagName()=="creature" &&
		rootEl.hasAttribute("intelligence");
}

/** @details
 * Ze zadaneho korenoveho elementu <em>creature</em> vytvori prototyp prisery
 * reprezentovane timto elementem. Pokud se pri zpracovani dokumentu
 * vyskytne chyba, zobrazi chybove hlaseni.
 * Zda jde o element se spravnym nazvem se jiz nezkouma, proto
 * by melo byt vzdy drive otestovano pomoci @c canHandle().
 * Predpoklada jiz nacteny zdrojovy obrazek v @c ResourceHandler.
 * @param rootEl korenovy element reprezentujici nacitanou priseru
 * @return Nove vytvoreny prototyp prisery (volajici se stava vlastnikem).
 * @retval 0 Doslo k chybe, prisera nemuze byt vytvorena.
 */
BombicMapObject * CreatureResourceHandler::createMapObject(
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

	return new BombicCreature(
		rootEl.attribute("name"), pixmap);
}

/**
 * @retval BombicMapObject::Creature Vzdy.
 */
BombicMapObject::Type CreatureResourceHandler::type() {
	return BombicMapObject::Creature;
}

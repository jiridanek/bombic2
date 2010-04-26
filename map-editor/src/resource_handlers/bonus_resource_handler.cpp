
#include <QPixmap>

#include <constants.h>

#include "bonus_resource_handler.h"

#include "../bombic/map_object.h"
#include "../bombic/bonus.h"

/** @details
 * Umi zpracovavat dokument s korenovym elementem <em>bonus</em>.
 * @param rootEl korenovy element dokumentu, ktery reprezentuje objekt mapy
 * @return Zda zpracovava takove dokumenty.
 */
bool BonusResourceHandler::canHandle(const QDomElement & rootEl) {
	return rootEl.tagName()=="bonus";
}

/** @details
 * Ze zadaneho korenoveho elementu <em>bonus</em> vytvori prototyp bonusu
 * reprezentovaneho timto elementem. Pokud se pri zpracovani dokumentu
 * vyskytne chyba, zobrazi chybove hlaseni.
 * Zda jde o element se spravnym nazvem se jiz nezkouma, proto
 * by melo byt vzdy drive otestovano pomoci @c canHandle().
 * Predpoklada jiz nacteny zdrojovy obrazek v @c ResourceHandler.
 * @param rootEl korenovy element reprezentujici nacitany bonus
 * @return Nove vytvoreny prototyp bonusu (volajici se stava vlastnikem).
 * @retval 0 Doslo k chybe, bonus nemuze byt vytvoren.
 */
BombicMapObject * BonusResourceHandler::createMapObject(
		const QDomElement & rootEl) {
	QDomElement imgEl;
	if(getSubElement(rootEl, imgEl)) {
		// get attributes
		int x = 0;
		int y = 0;
		if(!getAttrsXY(imgEl, x, y)) {
			return 0;
		}
		// get pixmap
		QPixmap pixmap = sourcePixmap().copy(
			x, y, CELL_SIZE, CELL_SIZE);

		return new BombicBonus(
			rootEl.attribute("name"), pixmap);
	} else {
		return 0;
	}
}

/**
 * @retval BombicMapObject::Bonus Vzdy.
 */
BombicMapObject::Type BonusResourceHandler::type() {
	return BombicMapObject::Bonus;
}

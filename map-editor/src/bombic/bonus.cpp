
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <constants.h>

#include "bonus.h"

/** @details
 * @param name nazev objektu
 * @param pixmap obrazek objektu
 */
BombicBonus::BombicBonus(const QString & name, const QPixmap & pixmap):
		BombicMapObject(name, pixmap) {
}

/**
 * @param srcBonus bonus, od ktereho kopirujeme
 */
BombicBonus::BombicBonus(BombicBonus & srcBonus):
		BombicMapObject(srcBonus) {
}

/**
 * @return Nove vytvorena kopie bonusu.
 */
BombicMapObject * BombicBonus::createCopy() {
	return new BombicBonus(*this);
}

/**
 * @retval Bonus Vzdy.
 */
BombicMapObject::Type BombicBonus::type() {
	return Bonus;
}

/** @details
 * Graficky prvek do sceny je vytvaren zde on demand,
 * pozicovan tak, aby horni levy bod bonusu byl v bode
 * @p position na scene. Souradnice z (z value) je nastavena
 * na souradnici y v polovine policka.
 * @param position pozadovana pozice ve scene
 * @return Graficky prvek umisteny na zadanou pozici.
 */
QGraphicsItem * BombicBonus::situateGraphicsItem(const QPointF & position) {
	if(!graphicsItem_) {
		// create new one
		// it will be destroyed in BombicMapObject destructor
		graphicsItem_ = new QGraphicsPixmapItem(pixmap_);
	}
	graphicsItem_->setPos(position);
	graphicsItem_->setZValue(position.y() + CELL_SIZE/2);

	/* DEBUG
	foreach(QGraphicsItem * i, graphicsItem_->children()) {
		i->hide();
	}
	new QGraphicsSimpleTextItem(
		QString::number(zValue), graphicsItem_);
	// */

	return graphicsItem_;
}

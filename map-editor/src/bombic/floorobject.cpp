
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <constants.h>

#include "floorobject.h"

/** @details
 * @param name nazev objektu
 * @param pixmap obrazek objektu
 * @param width sirska zdi v polickach
 * @param height vyska zdi v polickach
 */
BombicFloorobject::BombicFloorobject(const QString & name,
		const QPixmap & pixmap, int width, int height):
				BombicMapObject(name, pixmap) {
	rect_.setSize(QSize(width, height));
}

/**
 * @param srcFloorobject objekt na zemi, od ktereho kopirujeme
 */
BombicFloorobject::BombicFloorobject(BombicFloorobject & srcFloorobject):
		BombicMapObject(srcFloorobject) {
}

/**
 * @return Nove vytvorena kopie objektu na zemi.
 */
BombicMapObject * BombicFloorobject::createCopy() {
	return new BombicFloorobject(*this);
}

/**
 * @retval Floorobject Vzdy.
 */
BombicMapObject::Type BombicFloorobject::type() {
	return Floorobject;
}

/** @details
 * Graficky prvek do sceny je vytvaren zde on demand,
 * pozicovan tak, aby horni levy bod policka zabraneho objektem byl v bode
 * @p position na scene. Souradnice z (z value) je dana -1/2,
 * aby objekt prekryval prvky pozadi.
 * @param position pozadovana pozice ve scene
 * @return Graficky prvek umisteny na zadanou pozici.
 */
QGraphicsItem * BombicFloorobject::situateGraphicsItem(
		const QPointF & position) {
	if(!graphicsItem_) {
		// create new one
		// it will be destroyed in BombicMapObject destructor
		graphicsItem_ = new QGraphicsPixmapItem(pixmap_);
	}
	graphicsItem_->setPos(position);
	graphicsItem_->setZValue(-0.5);

	/* DEBUG
	foreach(QGraphicsItem * i, graphicsItem_->children()) {
		i->hide();
	}
	new QGraphicsSimpleTextItem(
		QString::number(zValue), graphicsItem_);
	// */

	return graphicsItem_;
}

/** @details
 * Objekt na zemi muze byt na jednom policku s priserou nebo hracem.
 * @param object porovnavany objekt
 * @return Zda lze (sebe) umistit na stejne policko s @p object.
 */
bool BombicFloorobject::canBeWith(BombicMapObject * object) {
	return object == this ||
		object->type() == Creature ||
		object->type() == Player;
}
/** @details
 * Objekt na zemi neblokuje generovani boxu.
 * @return Zda blokuje generovani beden.
 */
bool BombicFloorobject::blocksBoxGenerating() {
	return false;
}

/** @details
 * Objekt na zemi neblokuje generovani priser.
 * @return Zda blokuje generovani priser.
 */
bool BombicFloorobject::blocksCreatureGenerating() {
	return false;
}

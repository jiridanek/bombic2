
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <constants.h>

#include "creature.h"

/** @details
 * @param name nazev objektu
 * @param pixmap obrazek objektu
 */
BombicCreature::BombicCreature(const QString & name, const QPixmap & pixmap):
		BombicMapObject(name, pixmap) {
}

/**
 * @param srcCreature prisera, od ktere kopirujeme
 */
BombicCreature::BombicCreature(BombicCreature & srcCreature):
		BombicMapObject(srcCreature) {
}

/**
 * @return Nove vytvorena kopie prisery.
 */
BombicMapObject * BombicCreature::createCopy() {
	return new BombicCreature(*this);
}

/**
 * @retval Creature Vzdy.
 */
BombicMapObject::Type BombicCreature::type() {
	return Creature;
}

/** @details
 * Graficky prvek do sceny je vytvaren zde on demand,
 * pozicovan tak, aby horni levy bod policka zabraneho TODO byl v bode
 * @p position na scene. Souradnice z (z value) je dana horni souradnici
 * spodniho zabraneho policka.
 * @param position pozadovana pozice ve scene
 * @return Graficky prvek umisteny na zadanou pozici.
 */
QGraphicsItem * BombicCreature::situateGraphicsItem(const QPointF & position) {
	if(!graphicsItem_) {
		// create new one
		// it will be destroyed in BombicMapObject destructor
		graphicsItem_ = new QGraphicsPixmapItem(pixmap_);
		QPointF offset(CELL_SIZE - pixmap_.width(),
			CELL_SIZE - pixmap_.height() );
		graphicsItem_->setOffset(offset/2);
	}
	graphicsItem_->setPos(position);
	graphicsItem_->setZValue( position.y() );

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
 * Prisera muze byt na jednom policku pouze s priserou.
 * @param object porovnavany objekt
 * @return Zda lze (sebe) umistit na stejne policko s @p object.
 */
bool BombicCreature::canBeWith(BombicMapObject * object) {
	return object->type() == Creature ||
		object->type() == Floorobject;
}

/** @details
 * U prisery se mohou generovat jine prisery.
 * @return Zda blokuje generovani priser.
 */
bool BombicCreature::blocksCreatureGenerating() {
	return false;
}

/**
 * @return Zda ma byt na policku razena.
 */
bool BombicCreature::sortOnField() {
	return true;
}

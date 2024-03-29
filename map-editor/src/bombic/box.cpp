
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <constants.h>

#include "box.h"

/** @details
 * @param name nazev objektu
 * @param pixmap obrazek objektu
 * @param width sirska zdi v polickach
 * @param height vyska zdi v polickach
 * @param toplapping kolik policek zed prevysuje oblast, kterou zabira
 */
BombicBox::BombicBox(const QString & name, const QPixmap & pixmap,
		int width, int height, int toplapping):
				BombicMapObject(name, pixmap) {
	rect_.setSize(QSize(width, height));
	toplapping_ = toplapping;
}

/**
 * @param srcBox bedna, od ktere kopirujeme
 */
BombicBox::BombicBox(BombicBox & srcBox):
		BombicMapObject(srcBox) {
}

/**
 * @return Nove vytvorena kopie bedny.
 */
BombicMapObject * BombicBox::createCopy() {
	return new BombicBox(*this);
}

/**
 * @retval Box Vzdy.
 */
BombicMapObject::Type BombicBox::type() {
	return Box;
}

/** @details
 * Graficky prvek do sceny je vytvaren zde on demand,
 * pozicovan tak, aby horni levy bod policka zabraneho bednou byl v bode
 * @p position na scene. Souradnice z (z value) je dana horni souradnici
 * horniho zabraneho policka.
 * @param position pozadovana pozice ve scene
 * @return Graficky prvek umisteny na zadanou pozici.
 */
QGraphicsItem * BombicBox::situateGraphicsItem(const QPointF & position) {
	if(!graphicsItem_) {
		// create new one
		// it will be destroyed in BombicMapObject destructor
		graphicsItem_ = new QGraphicsPixmapItem(pixmap_);
		graphicsItem_->setOffset(0, -toplapping_*CELL_SIZE);
	}
	graphicsItem_->setPos(position);
	graphicsItem_->setZValue(position.y());

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
 * Bedna odblokovava generovani bonusu.
 * @return Zda odblokovava generovani bonusu.
 */
bool BombicBox::unblocksBonusGenerating() {
	return true;
}

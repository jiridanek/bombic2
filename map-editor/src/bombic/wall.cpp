
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <constants.h>

#include "wall.h"

/** @details
 * @param name nazev objektu
 * @param pixmap obrazek objektu
 * @param width sirska zdi v polickach
 * @param height vyska zdi v polickach
 * @param toplapping kolik policek zed prevysuje oblast, kterou zabira
 */
BombicWall::BombicWall(const QString & name, const QPixmap & pixmap,
		int width, int height, int toplapping):
				BombicMapObject(name, pixmap) {
	rect_.setSize(QSize(width, height));
	toplapping_ = toplapping;
}

/**
 * @param srcWall zed, od ktere kopirujeme
 */
BombicWall::BombicWall(BombicWall & srcWall):
		BombicMapObject(srcWall) {
}

/**
 * @return Nove vytvorena kopie zdi.
 */
BombicMapObject * BombicWall::createCopy() {
	return new BombicWall(*this);
}

/**
 * @retval Wall vzdy
 */
BombicMapObject::Type BombicWall::type() {
	return Wall;
}

/** @details
 * Graficky prvek do sceny je vytvaren zde on demand,
 * pozicovan tak, aby horni levy bod policka zabraneho zdi byl v bode
 * @p position na scene. Souradnice z (z value) je dana horni souradnici
 * spodniho zabraneho policka (u obvodovych zdi je jeste o pul bodu snizena,
 * abychom docilili efektu prekryti obvodove zdi a obycejne,
 * kdyz jsou pres sebe).
 * @param position pozadovana pozice ve scene
 * @return Graficky prvek umisteny na zadanou pozici.
 */
QGraphicsItem * BombicWall::situateGraphicsItem(const QPointF & position) {
	if(!graphicsItem_) {
		// create new one
		// it will be destroyed in BombicMapObject destructor
		graphicsItem_ = new QGraphicsPixmapItem(pixmap_);
		graphicsItem_->setOffset(0, -toplapping_*CELL_SIZE);
	}
	graphicsItem_->setPos(position);
	qreal zValue = position.y() + (rect_.height()-1)*CELL_SIZE;
	if(isBackground()) {
		zValue -= 0.5;
	}
	graphicsItem_->setZValue(zValue);

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
 * Zed muze byt na jednom policku opet jen se zdi,
 * a to jen zed pozadi s obycejnou.
 * @param object porovnavany objekt
 * @return Zda lze (sebe) umistit na stejne policko s @p object.
 */
bool BombicWall::canBeWith(BombicMapObject * object) {
	// wall can be on the same field only with another wall
	if(object->type() == Wall) {
		BombicWall * wallObj = static_cast<BombicWall *>(object);
		// but only if exactly one of them is background wall
		return wallObj==this
			|| (wallObj->isBackground() && !this->isBackground())
			|| (!wallObj->isBackground() && this->isBackground());
	} else {
		return false;
	}
}
/** @details
 * Nelze hybat obvodovymi zdmi.
 * @return Zda lze zdi hybat.
 */
bool BombicWall::canBeDragged() {
	return !isBackground();
}
/** @details
 * Nelze odstranit obvodovou zed.
 * @return Zda lze zed odstranit.
 */
bool BombicWall::canBeRemoved() {
	return !isBackground();
}
/**
 * @return Zda je zed obvodova (soucast pozadi).
 */
bool BombicWall::isBackground() {
	// anonymous (with no name) is the background wall
	return name_.isEmpty();
}

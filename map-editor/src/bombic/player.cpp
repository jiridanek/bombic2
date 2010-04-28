
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <constants.h>

#include "player.h"

/** @details
 * @param name nazev objektu
 * @param pixmap obrazek objektu
 */
BombicPlayer::BombicPlayer(const QString & name, const QPixmap & pixmap):
		BombicMapObject(name, pixmap) {
}

/**
 * @param srcPlayer hrac, od ktere kopirujeme
 */
BombicPlayer::BombicPlayer(BombicPlayer & srcPlayer):
		BombicMapObject(srcPlayer) {
}

/**
 * @return Nove vytvorena kopie hrace.
 */
BombicMapObject * BombicPlayer::createCopy() {
	return new BombicPlayer(*this);
}

/**
 * @retval Player Vzdy.
 */
BombicMapObject::Type BombicPlayer::type() {
	return Player;
}

/** @details
 * Graficky prvek do sceny je vytvaren zde on demand,
 * pozicovan tak, aby byl ve stredu policka jehoz levy horni bod je bode
 * @p position na scene. Souradnice z (z value) je dana y-novou souradnici
 * @p position.
 * @param position pozadovana pozice ve scene
 * @return Graficky prvek umisteny na zadanou pozici.
 */
QGraphicsItem * BombicPlayer::situateGraphicsItem(const QPointF & position) {
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
 * Hrac muze byt na jednom policku pouze s hracem a objektem na zemi.
 * @param object porovnavany objekt
 * @return Zda lze (sebe) umistit na stejne policko s @p object.
 */
bool BombicPlayer::canBeWith(BombicMapObject * object) {
	return object->type() == Player ||
		object->type() == Floorobject;
}

/**
 * Hrac nemuze byt nikdy odstranen z mapy (pouze premisten).
 * @return Zda ma byt odstranen z mapy.
 */
bool BombicPlayer::canBeRemoved() {
	return false;
}

/**
 * @return Zda ma byt na policku razena.
 */
bool BombicPlayer::sortOnField() {
	return true;
}

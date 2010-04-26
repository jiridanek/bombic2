
#include "map_object.h"

#include <QGraphicsPixmapItem>
#include <constants.h>

/** @details
 * @c thumbnail_ a @c graphicsItem_ jsou vytvareny on demand,
 * @c rect_ je defaultne nastaven na (0, 0, 1, 1) a @c toplapping_ na 0.
 * @param name nazev objektu
 * @param pixmap obrazek objektu
 */
BombicMapObject::BombicMapObject(const QString & name, const QPixmap & pixmap):
		name_(name), pixmap_(pixmap),
		thumbnail_(/* init it to null, it will be created on demand*/),
		graphicsItem_(0), rect_(0, 0, 1, 1), toplapping_(0) {
}

/** @details
 * Vsechny datove polozky jsou okopirovany copy-constructorem,
 * jen @c graphicsItem_ bude vytvorena on demand vlastni.
 * @param srcObject objekt jehoz kopii vytvarime.
 */
BombicMapObject::BombicMapObject(BombicMapObject & srcObject):
		name_(srcObject.name()), pixmap_(srcObject.pixmap()),
		thumbnail_(srcObject.thumbnail()),
		graphicsItem_(0), rect_(srcObject.rect()),
		toplapping_(srcObject.toplapping()) {
}

/** @details
 * Dealokuje graficky prvek (pokud je ve scene, automaticky je odstranen).
 */
BombicMapObject::~BombicMapObject() {
	if(graphicsItem_) {
		// there is some item - delete it
		delete graphicsItem_;
	}
}

/**
 * @return Nazev objektu.
 */
const QString & BombicMapObject::name() {
	return name_;
}

/**
 * @return Obrazek objektu.
 */
const QPixmap & BombicMapObject::pixmap() {
	return pixmap_;
}

/**
 * @return Nahled na objektu.
 */
const QPixmap & BombicMapObject::thumbnail() {
	if(thumbnail_.isNull()) {
		thumbnail_ = pixmap_.scaledToHeight(CELL_SIZE);
	}
	return thumbnail_;
}

/**
 * @return Obdelnik objektu (v polickach).
 */
const QRect & BombicMapObject::rect() {
	return rect_;
}

/**
 * @return Leve horni policko objektu v mape (je-li umisten v mape).
 * @warning Neni-li objekt umisten v mape, je hodnota nedefinovana
 * (muze byt jakakoli).
 */
BombicMap::Field BombicMapObject::field() {
	return rect_.topLeft();
}

/**
 * @param field leve horni policko, kam objekt umistujeme
 */
void BombicMapObject::setField(const BombicMap::Field & field) {
	rect_.moveTopLeft(field);
}

/**
 * @return Velikost objektu (v polickach).
 */
QSize BombicMapObject::size() {
	return rect_.size();
}

/**
 * @return Presah objektu (po ose y v polickach).
 */
int BombicMapObject::toplapping() {
	return toplapping_;
}

/** @details
 * Graficky prvek objektu tak, jak byl naposledy napozicovany.
 * Pokud prvek jeste napozicovany nebyl, vytvori se novy a pozicuje
 * na souradnice sceny (0, 0).
 * @return Graficky prvek objektu ve scene.
 */
QGraphicsItem * BombicMapObject::graphicsItem() {
	if(graphicsItem_) {
		return graphicsItem_;
	} else {
		return situateGraphicsItem(QPointF(0, 0));
	}
}

/** @details
 * Defaultne objekt nemuze byt s jinym objektem.
 * @return Zda muze byt s jinym objektem na tomtez policku.
 */
bool BombicMapObject::canBeWith(BombicMapObject * object) {
	return object==this;
}

/** @details
 * Defaultne objekt blokuje generovani boxu.
 * @return Zda blokuje generovani beden.
 */
bool BombicMapObject::blocksBoxGenerating() {
	return true;
}

/** @details
 * Defaultne objekt blokuje generovani priser.
 * @return Zda blokuje generovani priser.
 */
bool BombicMapObject::blocksCreatureGenerating() {
	return true;
}

/** @details
 * Defaultne objekt neodblokovava generovani bonusu.
 * @return Zda odblokovava generovani bonusu.
 */
bool BombicMapObject::unblocksBonusGenerating() {
	return false;
}

/** @details
 * Defaultne objekt muze byt presouvan.
 * @return Zda muze byt presouvan.
 */
bool BombicMapObject::canBeDragged() {
	return true;
}

/** @details
 * Defaultne objekt muze byt odstranen z mapy.
 * @return Zda muze byt odstranen z mapy.
 */
bool BombicMapObject::canBeRemoved() {
	return true;
}

/** @details
 * Razeni na policku znamena, ze objekt bude mirne vychylen z jeho
 * normalni pozice na policku, aby nebyl v zakrytu jineho objektu
 * na tomtez policku. To je zejmena pouzitelne u objektu, kterych
 * muze byt na policku najednou vice.
 * Objekt muze byt ale vychylen i kdyz je na policku sam.
 * Defaultne objekt nema byt razen.
 * @return Zda ma byt na policku razen.
 */
bool BombicMapObject::sortOnField() {
	return false;
}

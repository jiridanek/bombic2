
#include "generated_object.h"

#include <QGraphicsItem>
#include <constants.h>

/** @details
 * Defaultne je generovani povoleno a vizualizace zobrazena.
 * Graficky prvek do sceny by mel byt alokovan v potomcich
 * teto tridy.
 */
BombicGeneratedObject::BombicGeneratedObject(const BombicMap::Field & field):
		field_(field), labelGI_(0),
		hidden_(false), blocked_(false), allowed_(true) {

}

/** @details
 * Dealokuje graficky prvek.
 */
BombicGeneratedObject::~BombicGeneratedObject() {
	delete labelGI_;
}

void BombicGeneratedObject::show() {
	if(hidden_) {
		hidden_ = false;
		updateLabelVisibility();
	}
}

void BombicGeneratedObject::hide() {
	if(!hidden_) {
		hidden_ = true;
		updateLabelVisibility();
	}
}

void BombicGeneratedObject::block() {
	setBlocking(true);
}

void BombicGeneratedObject::unblock() {
	setBlocking(false);
}

void BombicGeneratedObject::allow() {
	setAllowance(true);
}

void BombicGeneratedObject::disallow() {
	setAllowance(false);
}

void BombicGeneratedObject::toggleAllowance() {
	setAllowance(!allowed_);
}

/**
 * @param block zda se ma generovani blokovat
 */
void BombicGeneratedObject::setBlocking(bool block) {
	if(block == blocked_) {
		return;
	}
	blocked_ = block;
	updateLabelVisibility();
	removeGeneratedObjects();
}

/**
 * @param allow zda se ma generovani povolit
 */
void BombicGeneratedObject::setAllowance(bool allow) {
	if(allow == allowed_) {
		return;
	}
	allowed_ = allow;
	updateLabelVisibility();
	removeGeneratedObjects();
}

/**
 * @return Zda je generovani blokovano jinym objektem.
 */
bool BombicGeneratedObject::blocked() {
	return blocked_;
}

/**
 * @return Zda je generovani povoleno.
 */
bool BombicGeneratedObject::allowed() {
	return allowed_;
}

/**
 * @return Zda lze generovat objekt (pouze podle povoleni a blokovani).
 */
bool BombicGeneratedObject::canGenerate() {
	return allowed_ && !blocked_;
}

/**
 */
void BombicGeneratedObject::addGeneratedObject(BombicMapObject * mapObject) {
	mapObject->setField(field_);
	mapObject->graphicsItem()->show();
	generatedObjects_.append(mapObject);
}


void BombicGeneratedObject::removeGeneratedObjects() {
	while(!generatedObjects_.isEmpty()) {
		BombicMapObject * o = generatedObjects_.takeFirst();
		o->graphicsItem()->hide();
		emit removingGeneratedObject(o);
	}

	emit canGenerateChanged();
}

/**
 * @return Graficky prvek vizualizace do sceny.
 */
QGraphicsItem * BombicGeneratedObject::graphicsItem() {
	return labelGI_;
}

/** TODO
 * @return Graficky prvek vizualizace do sceny.
 */
const BombicMap::Field & BombicGeneratedObject::field() {
	return field_;
}

/** TODO
 * @return Graficky prvek vizualizace do sceny.
 */
const BombicMap::ObjectListT & BombicGeneratedObject::generatedObjects() {
	return generatedObjects_;
}

/** @details
 * Nastavi pozici grafickeho prvku ve scene tak, aby byla
 * vizualizace spjata s dany polickem @p field.
 * @param field policko, pro ktere vizualizujeme
 */
void BombicGeneratedObject::setLabelPos() {
	// move to the field
	labelGI_->setPos(field_ * CELL_SIZE);
	// move by offset
	labelGI_->moveBy(
		( CELL_SIZE/2 - labelGI_->boundingRect().width() )/2.0, 3);
}

/** @details
 * Nastavi viditelnost grafickeho prvku podle aktualnich hodnot parametru.
 */
void BombicGeneratedObject::updateLabelVisibility() {
	labelGI_->setVisible(
		!hidden_ && allowed_ && !blocked_);
}

bool BombicGeneratedObject::blocksBoxGenerating() {
	return false;
}
bool BombicGeneratedObject::blocksCreatureGenerating() {
	return false;
}

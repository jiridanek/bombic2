
#include "map_object_generator.h"

#include <QGraphicsItem>
#include <constants.h>

/** @details
 * Defaultne je generovani povoleno a vizualizace zobrazena.
 * Graficky prvek do sceny by mel byt alokovan v potomcich
 * teto tridy.
 */
BombicMapObjectGenerator::BombicMapObjectGenerator(
		const BombicMap::Field & field):
				field_(field), labelGI_(0),
				hidden_(false),
				blocked_(false), allowed_(true) {

}

/** @details
 * Dealokuje graficky prvek.
 */
BombicMapObjectGenerator::~BombicMapObjectGenerator() {
	delete labelGI_;
}

void BombicMapObjectGenerator::show() {
	if(hidden_) {
		hidden_ = false;
		updateLabelVisibility();
	}
}

void BombicMapObjectGenerator::hide() {
	if(!hidden_) {
		hidden_ = true;
		updateLabelVisibility();
	}
}

void BombicMapObjectGenerator::block() {
	setBlocking(true);
}

void BombicMapObjectGenerator::unblock() {
	setBlocking(false);
}

void BombicMapObjectGenerator::allow() {
	setAllowance(true);
}

void BombicMapObjectGenerator::disallow() {
	setAllowance(false);
}

void BombicMapObjectGenerator::toggleAllowance() {
	setAllowance(!allowed_);
}

/**
 * @param block zda se ma generovani blokovat
 */
void BombicMapObjectGenerator::setBlocking(bool block) {
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
void BombicMapObjectGenerator::setAllowance(bool allow) {
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
bool BombicMapObjectGenerator::blocked() {
	return blocked_;
}

/**
 * @return Zda je generovani povoleno.
 */
bool BombicMapObjectGenerator::allowed() {
	return allowed_;
}

/**
 * @return Zda lze generovat objekt (pouze podle povoleni a blokovani).
 */
bool BombicMapObjectGenerator::canGenerate() {
	return allowed_ && !blocked_;
}

/**
 */
void BombicMapObjectGenerator::addGeneratedObject(BombicMapObject * mapObject) {
	mapObject->setField(field_);
	mapObject->graphicsItem()->show();
	generatedObjects_.append(mapObject);
}


void BombicMapObjectGenerator::removeGeneratedObjects() {
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
QGraphicsItem * BombicMapObjectGenerator::graphicsItem() {
	return labelGI_;
}

/** TODO
 * @return Graficky prvek vizualizace do sceny.
 */
const BombicMap::Field & BombicMapObjectGenerator::field() {
	return field_;
}

/** TODO
 * @return Graficky prvek vizualizace do sceny.
 */
const BombicMap::ObjectListT & BombicMapObjectGenerator::generatedObjects() {
	return generatedObjects_;
}

/** @details
 * Nastavi pozici grafickeho prvku ve scene tak, aby byla
 * vizualizace spjata s dany polickem @p field.
 * @param field policko, pro ktere vizualizujeme
 */
void BombicMapObjectGenerator::setLabelPos() {
	// move to the field
	labelGI_->setPos(field_ * CELL_SIZE);
	// move by offset
	labelGI_->moveBy(
		( CELL_SIZE/2 - labelGI_->boundingRect().width() )/2.0, 3);
}

/** @details
 * Nastavi viditelnost grafickeho prvku podle aktualnich hodnot parametru.
 */
void BombicMapObjectGenerator::updateLabelVisibility() {
	labelGI_->setVisible(
		!hidden_ && allowed_ && !blocked_);
}

bool BombicMapObjectGenerator::blocksBoxGenerating() {
	return false;
}
bool BombicMapObjectGenerator::blocksCreatureGenerating() {
	return false;
}


#include "generated_object.h"

#include <QGraphicsItem>
#include <constants.h>

/** @details
 * Defaultne je generovani povoleno a vizualizace zobrazena.
 * Graficky prvek do sceny by mel byt alokovan v potomcich
 * teto tridy.
 */
BombicGeneratedObject::BombicGeneratedObject(const BombicMap::Field field):
		field_(field), labelGI_(0),
		labelHidden_(false), objectHidden_(false),
		blocked_(false), allowed_(true) {

}

/** @details
 * Dealokuje graficky prvek.
 */
BombicGeneratedObject::~BombicGeneratedObject() {
	delete labelGI_;
}

void BombicGeneratedObject::showLabel() {
	if(labelHidden_) {
		labelHidden_ = false;
		updateLabelVisibility();
	}
}

void BombicGeneratedObject::hideLabel() {
	if(!labelHidden_) {
		labelHidden_ = true;
		updateLabelVisibility();
	}
}

void BombicGeneratedObject::showObject() {
	if(objectHidden_) {
		objectHidden_ = false;
		updateObjectsVisibility();
	}
}

void BombicGeneratedObject::hideObject() {
	if(!objectHidden_) {
		objectHidden_ = true;
		updateObjectsVisibility();
	}
}

void BombicGeneratedObject::block() {
	setBlocking(true);
}

void BombicGeneratedObject::unblock() {
	setBlocking(false);
}

/**
 * @param block zda se ma generovani blokovat
 */
void BombicGeneratedObject::setBlocking(bool block) {
	if(block == blocked_) {
		return;
	}
	blocked_ = block;
	if(blocked_ == !canGenerate()) {
		updateLabelVisibility();
		if(!canGenerate()) {
			// TODO discardGenerated();
		}
		emit canGenerateChanged();
	}
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
 * @param allow zda se ma generovani povolit
 */
void BombicGeneratedObject::setAllowance(bool allow) {
	if(allow == allowed_) {
		return;
	}
	allowed_ = allow;
	if(allowed_ == canGenerate()) {
		updateLabelVisibility();
		if(!canGenerate()) {
			// TODO discardGenerated();
		}
		emit canGenerateChanged();
	}
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
	generatedObjects_.append(mapObject);
	updateObjectsVisibility();
}

BombicMapObject * BombicGeneratedObject::takeGeneratedObject() {
	if(generatedObjects_.isEmpty()) {
		return 0;
	} else {
		BombicMapObject * o = generatedObjects_.takeLast();
		o->graphicsItem()->hide();
		return o;
	}
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
		!labelHidden_ && canGenerate() );
}

void BombicGeneratedObject::updateObjectsVisibility() {
	foreach(BombicMapObject * o, generatedObjects_) {
		o->graphicsItem()->setVisible(!objectHidden_);
	}
}

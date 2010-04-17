
#include "map_object_generator.h"

#include <QGraphicsItem>
#include <constants.h>

/** @details
 * Defaultne je generovani povoleno a vizualizace zobrazena.
 * Graficky prvek do sceny by mel byt alokovan v potomcich
 * teto tridy.
 * @param field na kterem policku objekty generuje
 */
MapObjectGenerator::MapObjectGenerator(
		const BombicMap::Field & field):
				field_(field), labelGI_(0),
				hidden_(false),
				blocked_(false), allowed_(true) {

}

/** @details
 * Dealokuje graficky prvek.
 */
MapObjectGenerator::~MapObjectGenerator() {
	delete labelGI_;
}

void MapObjectGenerator::show() {
	if(hidden_) {
		hidden_ = false;
		updateLabelVisibility();
	}
}

void MapObjectGenerator::hide() {
	if(!hidden_) {
		hidden_ = true;
		updateLabelVisibility();
	}
}

void MapObjectGenerator::block() {
	setBlocking(true);
}

void MapObjectGenerator::unblock() {
	setBlocking(false);
}

void MapObjectGenerator::allow() {
	setAllowance(true);
}

void MapObjectGenerator::disallow() {
	setAllowance(false);
}

void MapObjectGenerator::toggleAllowance() {
	setAllowance(!allowed_);
}

/**
 * @param block zda se ma generovani blokovat (jinym objektem)
 */
void MapObjectGenerator::setBlocking(bool block) {
	if(block == blocked_) {
		return;
	}
	blocked_ = block;
	emit canGenerateChanged();
	updateLabelVisibility();
	// if blocked_ changed to false
	// there weren't any generated objects
	// so we can remove "them"
	removeGeneratedObjects();
}

/**
 * @param allow zda se ma generovani povolit
 */
void MapObjectGenerator::setAllowance(bool allow) {
	if(allow == allowed_) {
		return;
	}
	allowed_ = allow;
	emit canGenerateChanged();
	updateLabelVisibility();
	// if allowed_ changed to true
	// there weren't any generated objects
	// so we can remove "them"
	removeGeneratedObjects();
}

/**
 * @return Zda je generovani blokovano jinym objektem.
 */
bool MapObjectGenerator::blocked() {
	return blocked_;
}

/**
 * @return Zda je generovani povoleno.
 */
bool MapObjectGenerator::allowed() {
	return allowed_;
}

/**
 * @return Zda lze generovat objekt (pouze podle povoleni a blokovani).
 */
bool MapObjectGenerator::canGenerate() {
	return allowed_ && !blocked_;
}

/** @details
 * Zde neni kontrolovano, jestli je mozne objekt vygenerovat,
 * proto by melo byt vzdy pouzito canGenerate().
 * @param mapObject objekty, ktery ma byt vygenerovan
 */
void MapObjectGenerator::addGeneratedObject(
		BombicMapObject * mapObject) {
	mapObject->setField(field_);
	mapObject->graphicsItem()->show();
	generatedObjects_.append(mapObject);
}

/** @details
 * Odstrani postupne vsechny generovane objekty a pro kazdy
 * vyvola signal removingGeneratedObject().
 * Nakonec vyvola signal canGenerateChanged(), kdyby se nahodou
 * timto ukonem neco zmenilo.
 */
void MapObjectGenerator::removeGeneratedObjects() {
	while(!generatedObjects_.isEmpty()) {
		BombicMapObject * o = generatedObjects_.takeFirst();
		o->graphicsItem()->hide();
		emit removingGeneratedObject(o);
	}

	emit canGenerateChanged();
}

/**
 * @return Graficky prvek popisku generatoru do sceny.
 */
QGraphicsItem * MapObjectGenerator::graphicsItem() {
	return labelGI_;
}

/**
 * @return Na kterem policku generuje.
 */
const BombicMap::Field & MapObjectGenerator::field() {
	return field_;
}

/**
 * @return Seznam aktualne generovanych objektu.
 */
const BombicMap::ObjectListT & MapObjectGenerator::generatedObjects() {
	return generatedObjects_;
}

/** @details
 * Nastavi pozici grafickeho prvku ve scene tak, aby byla
 * vizualizace spjata s polickem, na ktere generuje.
 */
void MapObjectGenerator::setLabelPos() {
	// move to the field
	labelGI_->setPos(field_ * CELL_SIZE);
	// move by offset
	labelGI_->moveBy(
		( CELL_SIZE/2 - labelGI_->boundingRect().width() )/2.0, 3);
}

/** @details
 * Nastavi viditelnost grafickeho prvku podle aktualnich hodnot parametru.
 */
void MapObjectGenerator::updateLabelVisibility() {
	labelGI_->setVisible(
		!hidden_ && allowed_ && !blocked_);
}

/**
 * @return Zda blokuje generovani beden.
 */
bool MapObjectGenerator::blocksBoxGenerating() {
	return false;
}
/**
 * @return Zda blokuje generovani priser.
 */
bool MapObjectGenerator::blocksCreatureGenerating() {
	return false;
}


#include "generated_object.h"

#include <QGraphicsItem>
#include <constants.h>

/** @details
 * Defaultne je generovani povoleno a vizualizace zobrazena.
 * Graficky prvek do sceny by mel byt alokovan v potomcich
 * teto tridy.
 */
BombicGeneratedObject::BombicGeneratedObject():
		graphicsItem_(0), allowed_(true), hidden_(false) {

}

/** @details
 * Dealokuje graficky prvek.
 */
BombicGeneratedObject::~BombicGeneratedObject() {
	delete graphicsItem_;
}

void BombicGeneratedObject::show() {
	if(allowed_ && hidden_) {
		graphicsItem_->show();
	}
	hidden_ = false;
}

void BombicGeneratedObject::hide() {
	if(!hidden_) {
		graphicsItem_->hide();
		hidden_ = true;
	}
}

void BombicGeneratedObject::allow() {
	if(!allowed_ && !hidden_) {
		graphicsItem_->show();
	}
	allowed_ = true;
}

void BombicGeneratedObject::disallow() {
	if(allowed_ && !hidden_) {
		graphicsItem_->hide();
	}
	allowed_ = false;
}

/**
 * @return Zda je generovani povoleno.
 */
bool BombicGeneratedObject::allowed() {
	return allowed_;
}

/**
 * @return Graficky prvek vizualizace do sceny.
 */
QGraphicsItem * BombicGeneratedObject::graphicsItem() {
	return graphicsItem_;
}

/** @details
 * Nastavi pozici grafickeho prvku ve scene tak, aby byla
 * vizualizace spjata s dany polickem @p field.
 * @param field policko, pro ktere vizualizujeme
 */
void BombicGeneratedObject::setPos(const BombicMap::Field & field) {
	// move to the field
	graphicsItem_->setPos(field*CELL_SIZE);
	// move by offset
	graphicsItem_->moveBy(
		( CELL_SIZE/2 - graphicsItem_->boundingRect().width() )/2.0, 3);
}


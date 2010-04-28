
#include "creature_generator.h"

#include <QGraphicsItem>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsEllipseItem>
#include <QRadialGradient>
#include <QBrush>
#include <QPen>
#include <QColor>
#include <QPointF>
#include <constants.h>

/** @details
 * Vytvori graficky prvek vizualizace pro policko @p field,
 * ktery se nachazi v prave polovine tohoto policka.
 * @param field policko mapy, pro ktere chceme vizualizovat
 */
CreatureGenerator::CreatureGenerator(
		const BombicMap::Field & field):
				MapObjectGenerator(field) {
	QGraphicsEllipseItem * bgItem =
		new QGraphicsEllipseItem(0, 0,
			GENERATOR_BACKGROUND_SIZE, GENERATOR_BACKGROUND_SIZE);
	#define BG_GRAD_RADIUS (GENERATOR_BACKGROUND_SIZE/2)
	QRadialGradient grad(QPointF(BG_GRAD_RADIUS, BG_GRAD_RADIUS),
		BG_GRAD_RADIUS);
	#undef BG_GRAD_RADIUS
	grad.setColorAt(0, GENERATOR_BACKGROUND_FROM_COLOR);
	grad.setColorAt(1, GENERATOR_BACKGROUND_TO_COLOR);
	bgItem->setBrush(grad);
	bgItem->setPen(QPen(Qt::NoPen));

	QGraphicsSimpleTextItem * textItem =
		new QGraphicsSimpleTextItem("C", bgItem);
	// center the text item in the background
	textItem->setPos(
		( bgItem->rect().bottomRight() -
			textItem->boundingRect().bottomRight() )/2.0 );

	labelGI_ = bgItem;

	setLabelPos();
	// move to the right half of field
	labelGI_->moveBy(CELL_SIZE/2, 0);
}

/**
 * @return Typ objektu, ktery generuje.
 */
BombicMapObject::Type CreatureGenerator::type() {
	return BombicMapObject::Creature;
}

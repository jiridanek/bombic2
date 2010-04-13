
#include "generated_box.h"

#include <QGraphicsSimpleTextItem>
#include <constants.h>

/** @details
 * Vytvori graficky prvek vizualizace pro policko @p field,
 * ktery se nachazi v leve polovine tohoto policka.
 * @param field policko mapy, pro ktere chceme vizualizovat
 */
BombicGeneratedBox::BombicGeneratedBox(const BombicMap::Field & field) {
	labelGI_ = new QGraphicsSimpleTextItem("B");
	// TODO set font

	setPos(field);
}

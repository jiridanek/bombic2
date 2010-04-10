
#include <QMouseEvent>

#include "map_field_view_object.h"

#include "map_view.h"
#include "bombic/map_object.h"

MapFieldViewObject::MapFieldViewObject(
		BombicMapObject * mapObject, QWidget * parent):
				QLabel(parent), object_(mapObject) {
	setPixmap(mapObject->thumbnail());
}

/** @details
 * Pri zmacknutem tlacitku mysi nastartuje tazeni objektu.
 * @param event udalost, ktera handler vyvolala
 */
void MapFieldViewObject::mouseMoveEvent(QMouseEvent * event) {
	if(event->buttons().testFlag(Qt::LeftButton)) {
		if(object_->canBeDragged()) {
			MapView::execDragging(this, object_);
		}
	}
}

/** @details
 * Odstrani ze sceny (a mapy) reprezentovany objekt.
 * @param event udalost, ktera handler vyvolala
 */
void MapFieldViewObject::mouseDoubleClickEvent(QMouseEvent * event) {
	switch(event->button()) {
		case Qt::LeftButton:
// 			removeClickedObject(event);
			break;
		default:
			// nothing to do
			break;
	}
}

/** @details
 * Odstrani (a dealokuje) reprezentovany objekt je-li to povoleno.
 * /
void MapFieldViewObject::remove() {
	if(!object_->canBeRemoved()) {
		return;
	}
	remove(clickedObj);
	delete clickedObj;
}
*/
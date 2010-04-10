
#include <QMouseEvent>
#include <QGraphicsItem>

#include "map_field_view_object.h"

#include "map_view.h"
#include "map_scene.h"
#include "bombic/map_object.h"

/** @details
 * Nastavi obrazek objektu.
 * @param mapObject objekt mapy, ktery chceme vizualizovat
 * @param parent rodicovsky widget
 */
MapFieldViewObject::MapFieldViewObject(
		BombicMapObject * mapObject, QWidget * parent):
				QLabel(parent), object_(mapObject) {
	setPixmap(mapObject->thumbnail());
}

/** @details
 * Pri zmacknutem tlacitku mysi nastartuje tazeni objektu (pokud je to mozne).
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
 * Odstrani ze sceny (a mapy) reprezentovany objekt (pokud je to mozne).
 * @param event udalost, ktera handler vyvolala
 */
void MapFieldViewObject::mouseDoubleClickEvent(QMouseEvent * event) {
	switch(event->button()) {
		case Qt::LeftButton:
			if(object_->canBeRemoved() &&
					object_->graphicsItem()->scene()) {
				static_cast<MapScene *>(
					object_->graphicsItem()->scene() )
						->remove(object_);
				delete object_;
				MAP_VIEW->updateFieldView();
			}
			break;
		default:
			// nothing to do
			break;
	}
}

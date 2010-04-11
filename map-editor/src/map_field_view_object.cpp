
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
		if(object_ && object_->canBeDragged()) {
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
			if(object_ && object_->canBeRemoved() &&
					object_->graphicsItem()->scene()) {
				// we have scene and object to remove
				MapScene * scene = static_cast<MapScene *>(
					object_->graphicsItem()->scene() );
				// so we remove it
				scene->remove(object_);
				// update the view
				MAP_VIEW->updateFieldView();
				// and delete the object - I hope it is now ok
				delete object_;
				object_ = 0;
			}
			break;
		default:
			// nothing to do
			break;
	}
}

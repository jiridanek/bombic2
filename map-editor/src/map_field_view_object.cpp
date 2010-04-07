
#include "map_field_view_object.h"

#include "bombic/map_object.h"

MapFieldViewObject::MapFieldViewObject(
		BombicMapObject * mapObject, QWidget * parent):
				QLabel(parent), object_(mapObject) {
	setAutoFillBackground(true);
	setPixmap(mapObject->thumbnail());
}

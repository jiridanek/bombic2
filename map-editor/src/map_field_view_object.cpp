
#include "map_field_view_object.h"

MapFieldViewObject::MapFieldViewObject(
		BombicMapObject * mapObject, QWidget * parent):
				QLabel(parent), object_(mapObject) {
	setText("label");
}

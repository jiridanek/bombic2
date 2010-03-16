
#include "map.h"

#include "map_background.h"
#include "map_object.h"

BombicMap::BombicMap(int width, int height,
		BombicMapBackground * background):
			fieldsRect_(0, 0, width, height),
			background_(background) {
	FieldsT::value_type::value_type emptyList;
	FieldsT::value_type column(height, emptyList);
	fields_ = FieldsT(width, column);
}

BombicMap::~BombicMap() {
}

void BombicMap::insert(BombicMapObject * object,
		const BombicMap::Field & dstField) {
	QRect objRect(dstField, object->size());
	if(!fieldsRect_.contains(objRect)) {
		return;
	}
	int top = objRect.top();
	int bottom = objRect.bottom();
	int left = objRect.left();
	int right = objRect.right();
	// for every field occupied by object
	for(int x = left ; x <= right ; ++x) {
		for(int y = top ; y <= bottom ; ++y) {
			// insert the object in the map
			fields_[x][y].append(object);
		}
	}
}

bool BombicMap::canInsert(BombicMapObject * object,
		const BombicMap::Field & dstField) {
	QRect objRect(dstField, object->size());
	if(!fieldsRect_.contains(objRect)) {
		return false;
	}
	int top = objRect.top();
	int bottom = objRect.bottom();
	int left = objRect.left();
	int right = objRect.right();
	// for every field occupied by object
	for(int x = left ; x <= right ; ++x) {
		for(int y = top ; y <= bottom ; ++y) {
			// check, whether there isn't other object,
			// that exludes this one
			foreach(BombicMapObject * otherObj, fields_[x][y]) {
				if(!object->canBeWith(otherObj)) {
					return false;
				}
			}
		}
	}
	return true;
}

const QRect & BombicMap::fieldsRect() {
	return fieldsRect_;
}

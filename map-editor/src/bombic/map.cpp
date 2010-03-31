
#include "map.h"

#include "map_background.h"
#include "map_object.h"
#include "generated_box.h"
#include "generated_creature.h"

BombicMap::BombicMap(int width, int height,
		BombicMapBackground * background):
			fieldsRect_(0, 0, width, height),
			background_(background) {
	FieldsT::value_type::value_type emptyFieldSet;
	FieldsT::value_type column(height, emptyFieldSet);
	fields_ = FieldsT(width, column);
	for(int x = 0 ; x < width ; ++x) {
		for(int y = 0 ; y < height ; ++y) {
			fields_[x][y].genBox =
				new BombicGeneratedBox(Field(x, y));
			fields_[x][y].genCreature =
				new BombicGeneratedCreature(Field(x, y));
		}
	}
}

BombicMap::~BombicMap() {
	for(int x = fieldsRect_.left() ; x <= fieldsRect_.right() ; ++x) {
		for(int y = fieldsRect_.top() ; y <= fieldsRect_.bottom() ; ++y) {
			fields_[x][y].genBox =
				new BombicGeneratedBox(Field(x, y));
			fields_[x][y].genCreature =
				new BombicGeneratedCreature(Field(x, y));
		}
	}
}

bool BombicMap::canInsert(BombicMapObject * object,
		const BombicMap::Field & dstField) {
	QRect objRect(dstField, object->size());
	QRect toplappingRect = objRect.adjusted(0, -object->toplapping(), 0, 0);
	if(!fieldsRect_.contains(toplappingRect)) {
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
			foreach(BombicMapObject * otherObj,
					fields_[x][y].objList) {
				if(!object->canBeWith(otherObj)) {
					return false;
				}
			}
		}
	}
	return true;
}

void BombicMap::insert(BombicMapObject * object,
		const BombicMap::Field & dstField) {
	object->setField(dstField);
	QRect objRect = object->rect();
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
			if(object->type()==BombicMapObject::Floorobject) {
				fields_[x][y].objList.prepend(object);
			} else {
				fields_[x][y].objList.append(object);
			}
			// hide generating labels if it is blocker
			if(object->blocksBoxGenerating()) {
				fields_[x][y].genBox->hide();
			}
			if(object->blocksCreatureGenerating()) {
				fields_[x][y].genCreature->hide();
			}
		}
	}
}

void BombicMap::remove(BombicMapObject * object) {
	QRect objRect = object->rect();
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
			// remove the object from the map
			fields_[x][y].objList.removeAll(object);
			// show generating labels if there isn̈́'t blocker
			bool blockBoxGenerating = false;
			bool blockCreatureGenerating = false;
			// find the blocker
			foreach(BombicMapObject * o, fields_[x][y].objList) {
				if(o->blocksBoxGenerating()) {
					blockBoxGenerating = true;
				}
				if(object->blocksCreatureGenerating()) {
					blockCreatureGenerating = true;
				}
			}
			if(!blockBoxGenerating) {
				fields_[x][y].genBox->show();
			}
			if(!blockCreatureGenerating) {
				fields_[x][y].genCreature->show();
			}
		}
	}
}

BombicMapObject * BombicMap::objectOnTop(const BombicMap::Field & field) {
	if(!fieldsRect_.contains(field)) {
		return 0;
	}
	if(fields_[field.x()][field.y()].objList.isEmpty()) {
		return 0;
	}
	return fields_[field.x()][field.y()].objList.last();
}

BombicGeneratedObject * BombicMap::generatedBox(
		const BombicMap::Field & field) {
	if(!fieldsRect_.contains(field)) {
		return 0;
	}
	return fields_[field.x()][field.y()].genBox;
}

BombicGeneratedObject * BombicMap::generatedCreature(
		const BombicMap::Field & field) {
	if(!fieldsRect_.contains(field)) {
		return 0;
	}
	return fields_[field.x()][field.y()].genCreature;
}

const QRect & BombicMap::fieldsRect() {
	return fieldsRect_;
}

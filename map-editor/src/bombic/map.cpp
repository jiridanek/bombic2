
#include "map.h"

#include "map_background.h"
#include "map_object.h"
#include "wall.h"
#include "box_generator.h"
#include "creature_generator.h"

/** @details
 * Zkonstruuje mapu o rozmerech @p width, @p height s pozadim @p background.
 * Nova mapa se stava vlastnikem @p background a to bude dealokovano
 * pri destrukci mapy.
 * Nove zkonstruovana mapa obsahuje pouze obvodove zdi pozadi a inicializovane
 * generoty objektu (vsechna policka povolena pro generovani).
 * @param width sirska mapy v polickach
 * @param height vyska mapy v polickach
 * @param background pointer na pozadi
 */
BombicMap::BombicMap(int width, int height,
		BombicMapBackground * background):
			fieldsRect_(0, 0, width, height),
			background_(background) {
	// construct fields area
	FieldsT::value_type::value_type emptyFieldSet;
	FieldsT::value_type column(height, emptyFieldSet);
	fields_ = FieldsT(width, column);
	// init generators
	for(int x = 0 ; x < width ; ++x) {
		for(int y = 0 ; y < height ; ++y) {
			fields_[x][y].boxGen =
				new BombicBoxGenerator(Field(x, y));
			fields_[x][y].creatureGen =
				new BombicCreatureGenerator(Field(x, y));
		}
	}
	// background walls
	insertBackgroundWalls();
}

/** @details
 * Do mapy vlozi obvodove zdi definovane pozadim @c background_.
 */
void BombicMap::insertBackgroundWalls() {

	#define PREPARE_WALL_AND_RECT(situation, moveMethod, getMethod) \
		BombicWall * wall = background_->getWall( \
			BombicMapBackground::situation); \
		/* create wall rect with right size (anywhere) */ \
		QRect wallRect(QPoint(0,0), wall->size()); \
		/* move the wall rect to the corner of map */ \
		wallRect.moveMethod(fieldsRect_.getMethod());

	// corners
	#define CORNER(situation, moveMethod, getMethod) \
		do { \
			PREPARE_WALL_AND_RECT(situation, moveMethod, getMethod) \
			/* insert wall (by the top left corner of wall rect) */ \
			insert(wall->createCopy(), wallRect.topLeft() ); \
		} while(0)
	CORNER(TopLeft, moveTopLeft, topLeft);
	CORNER(TopRight, moveTopRight, topRight);
	CORNER(BottomLeft, moveBottomLeft, bottomLeft);
	CORNER(BottomRight, moveBottomRight, bottomRight);
	#undef CORNER
	// whole sides
	#define SIDE(situation, moveMethod, getMethod, stepX, stepY) \
		do { \
			PREPARE_WALL_AND_RECT(situation, moveMethod, getMethod) \
			/* along whole side */ \
			for(BombicMap::Field field = wallRect.topLeft() ; \
					fieldsRect_.contains(field) ; \
					field += BombicMap::Field(stepX, stepY) ) { \
				/* insert wall if it is possible */ \
				if(canInsert(wall, field)) { \
					insert(wall->createCopy(), field); \
				} \
			} \
		} while(0)
	SIDE(Top, moveTopLeft, topLeft, 1, 0);
	SIDE(Left, moveTopLeft, topLeft, 0, 1);
	SIDE(Right, moveTopRight, topRight, 0, 1);
	SIDE(Bottom, moveBottomLeft, bottomLeft, 1, 0);
	#undef SIDE
	#undef PREPARE_WALL_AND_RECT
}

/** @details
 * Dealokuje vsechny objekty mapy a pozadi.
 */
BombicMap::~BombicMap() {
	for(int x = fieldsRect_.left() ; x <= fieldsRect_.right() ; ++x) {
		for(int y = fieldsRect_.top() ; y <= fieldsRect_.bottom() ; ++y) {
			delete fields_[x][y].boxGen;
			delete fields_[x][y].creatureGen;
			foreach(BombicMapObject * o, fields_[x][y].objList) {
				// TODO remove first the object from other fields
				delete o;
			}
		}
	}
	delete background_;
}

/** @details
 * Kontroluje, zda se objekt vejde do mapy (podle BombicMapObject::size()
 * a BombicMapObject::toplapping()) a zda muze byt vkladany objekt na policku
 * zaroven s objekty, ktere jiz na polickach, do kterych objekt zasahuje
 * (BombicMapObject::canBeWith()).
 * @param object vkladany objekt
 * @param dstField policko, na ktere chceme vlozit objekt (leve horni objektu)
 * @return Zda muze byt objekt vlozen.
 */
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

/** @details
 * Vlozi objekt do mapy. Kontroluje pouze, jestli se objekt do mapy vejde.
 * Nekontroluje toplapping objektu, a ostatni objekty na polick zasazenych
 * polickach (viz BombicMap::canInsert()).
 * Prebira vlastnictvi @p object, ktery bude dealokovan v destruktoru mapy.
 * @param object vkladany objekt
 * @param dstField policko, na ktere chceme vlozit objekt (leve horni objektu)
 */
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
			// block generators if it is blocker
			if(object->blocksBoxGenerating()) {
				fields_[x][y].boxGen->block();
			}
			if(object->blocksCreatureGenerating()) {
				fields_[x][y].creatureGen->block();
			}
		}
	}
}

/** @details
 * Odstrani objekt ze vsech policek, na kterych lezi.
 * Vlastnictvi objektu prechazi na volajiciho, objekt jiz
 * nebude dealokovan v destruktoru mapy.
 * @param object odstranovany objekt
 */
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
			updateGeneratorsBlocking(fields_[x][y]);
		}
	}
}

void BombicMap::updateGeneratorsBlocking(const Field & field) {
	if(fieldsRect_.contains(field)) {
		updateGeneratorsBlocking(
			fields_[field.x()][field.y()]);
	}
}

void BombicMap::updateGeneratorsBlocking(const FieldSetT & fieldSet) {
	// check if some generator is blocking some other one
	bool blockBoxes =
		fieldSet.boxGen->blocksBoxGenerating() ||
		fieldSet.creatureGen->blocksBoxGenerating();
	bool blockCreatures =
		fieldSet.boxGen->blocksCreatureGenerating() ||
		fieldSet.creatureGen->blocksCreatureGenerating();
	// find the blocker
	foreach(BombicMapObject * o, fieldSet.objList) {
		if(o->blocksBoxGenerating()) {
			blockBoxes = true;
		}
		if(o->blocksCreatureGenerating()) {
			blockCreatures = true;
		}
	}
	fieldSet.boxGen->setBlocking(blockBoxes);
	fieldSet.creatureGen->setBlocking(blockCreatures);
}

/** @details
 * Pokud je policko @p field a neni prazdne, ziska vrchni objekt na policku.
 * Poradi objektu na policku je dano poradim pridavani do mapy (LIFO).
 * Vyjimku tvori floorobject (objekt na zemi), ktery je v dusledku soucasti
 * pozadi, proto se do mapy pridava jako spodni prvek policka.
 * @param field policko, jehoz objekt chceme ziskat
 * @return Objekt, ktery lezi navrchu policka.
 * @retval 0 Policko neni v mape nebo je prazdne.
 */
BombicMapObject * BombicMap::objectOnTop(const BombicMap::Field & field) {
	if(!fieldsRect_.contains(field)) {
		return 0;
	}
	if(fields_[field.x()][field.y()].objList.isEmpty()) {
		return 0;
	}
	return fields_[field.x()][field.y()].objList.last();
}

/** @ details
 * Seznam objektu na policku (objekty zustavaji vlastnictvim mapy).
 * Poradi objektu v seznamu je dano poradim na policku, zpusobene
 * poradim pridavani do mapy.
 * @param field policko, jehoz objekty chceme ziskat
 * @return Seznam objektu na policku.
 * @warning @p field musi byt uvnitr mapy - neni provadena zadna kontrola
 */
const BombicMap::ObjectListT & BombicMap::objectsOnField(const BombicMap::Field & field) {
	return fields_[field.x()][field.y()].objList;
}

/** @details
 * Na kazdem policku vedeme zaznam o tom, zda lze na tomto
 * policku generovat bedny. Toto je dotaz na strukturu konkretniho policka.
 * Vracena struktura zustava ve vlastnictvi mapy.
 * @param field policko, o nemz chceme informace
 * @return Generator beden.
 * @retval 0 Zadane policko neni v mape.
 */
BombicMapObjectGenerator * BombicMap::boxGenerator(
		const BombicMap::Field & field) {
	if(!fieldsRect_.contains(field)) {
		return 0;
	}
	return fields_[field.x()][field.y()].boxGen;
}

/** @details
 * Na kazdem policku vedeme zaznam o tom, zda lze na tomto
 * policku generovat prisery. Toto je dotaz na strukturu konkretniho policka.
 * Vracena struktura zustava ve vlastnictvi mapy.
 * @param field policko, o nemz chceme informace
 * @return Generator priser.
 * @retval 0 Zadane policko neni v mape.
 */
BombicMapObjectGenerator * BombicMap::creatureGenerator(
		const BombicMap::Field & field) {
	if(!fieldsRect_.contains(field)) {
		return 0;
	}
	return fields_[field.x()][field.y()].creatureGen;
}

const BombicMap::ObjectListT & BombicMap::generatedBoxes() {
	return generatedBoxes_;
}
const BombicMap::ObjectListT & BombicMap::generatedCreatures() {
	return generatedCreatures_;
}


void BombicMap::setGeneratedBoxesCount(BombicMapObject * box, int count) {
	setGeneratedObjectsCount(generatedBoxes_, box, count);
}

void BombicMap::setGeneratedCreaturesCount(BombicMapObject * creature,
		int count) {
	setGeneratedObjectsCount(generatedCreatures_, creature, count);
}

void BombicMap::setGeneratedObjectsCount(ObjectListT & objList,
		BombicMapObject * object, int count) {
	// current count
	ObjectListT objects;
	foreach(BombicMapObject * o, objList) {
		if(o->name() == object->name()) {
			objects.append(o);
		}
	}
	int diffCount = count - objects.size();
	if(diffCount < 0) {
		// we need to remove some objects
		for(int i = 0 ; i > diffCount ; --i) {
			BombicMapObject * o =
				objects.takeAt(qrand() % objects.size());
			removeGeneratedMapObject(objList, o);
			delete o;
		}
	} else {
		// we need to add some objects
		for(int i = 0 ; i < diffCount ; ++i) {
			addGeneratedMapObject(objList, object->createCopy());
		}
	}
}

void BombicMap::addGeneratedMapObject(ObjectListT & objList,
		BombicMapObject * object) {
	objList.append(object);
	// TODO emit generatedMapObjectAdded(object);
}

void BombicMap::removeGeneratedMapObject(ObjectListT & objList,
		BombicMapObject * object) {
	Field objField = object->field();
	if(fieldsRect_.contains(objField)) {
		// the object may be generated somewhere in map
		// TODO remove from map
	}
	objList.removeAll(object);
}


/**
 * @return Obdelnik mapy (v jednotkach policek).
 */
const QRect & BombicMap::fieldsRect() {
	return fieldsRect_;
}
/** @details
 * Vraceny pointer zustava ve vlastnictvi mapy a bude dealokovan
 * v destruktoru mapy.
 * @return Pozadi mapy.
 */
BombicMapBackground * BombicMap::background() {
	return background_;
}

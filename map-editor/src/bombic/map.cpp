
#include <QApplication>

#include "map.h"

#include "map_background.h"
#include "map_object.h"
#include "wall.h"
#include "../generators/box_generator.h"
#include "../generators/creature_generator.h"
#include "../generators/bonus_generator.h"

#include "../resource_handlers/map_resource_handler.h"

/** @details
 * Zkonstruuje mapu o rozmerech @p width, @p height s pozadim @p background.
 * Nova mapa se stava vlastnikem @p background a to bude dealokovano
 * pri destrukci mapy.
 * Nove zkonstruovana mapa obsahuje pouze obvodove zdi pozadi a inicializovane
 * generatory objektu (vsechna policka povolena pro generovani).
 * @param width sirska mapy v polickach
 * @param height vyska mapy v polickach
 * @param background pointer na pozadi
 */
BombicMap::BombicMap(const QString & name, int width, int height,
		BombicMapBackground * background, const QString & filename):
				name_(name), filename_(filename),
				fieldsRect_(0, 0, width, height),
				background_(background), modified_(true) {
	// construct fields area
	FieldsT::value_type::value_type emptyFieldSet;
	FieldsT::value_type column(height, emptyFieldSet);
	fields_ = FieldsT(width, column);
	// init generators
	#define GENERATOR(className, fieldSetMember) \
		do { \
			fields_[x][y].fieldSetMember = \
				new className(Field(x, y)); \
			connect(fields_[x][y].fieldSetMember, \
				SIGNAL(allowanceChanged()), \
				this, SLOT(setModified()) ); \
		} while(0)

	for(int x = 0 ; x < width ; ++x) {
		for(int y = 0 ; y < height ; ++y) {
			GENERATOR(BoxGenerator, boxGen);
			GENERATOR(CreatureGenerator, creatureGen);
			GENERATOR(BonusGenerator, bonusGen);
		}
	}
	#undef GENERATOR

	// background walls
	insertBackgroundWalls();

}

/**
 * @param newBackground nazev noveho pozadi
 * @return Nove alokovana mapa s novym pozadim.
 * @retval 0 Mapu nebylo mozne vytvorit.
 */
BombicMap * BombicMap::createCopy(const QString & newBackground) {
	MapResourceHandler mapRH;
	return mapRH.createMapCopy(this,
		fieldsRect_.width(), fieldsRect_.height(), newBackground);
}

/**
 * @param newWidth nova sirka mapy
 * @param newHeight nova vyska mapy
 * @return Nove alokovana mapa s novymi rozmery.
 * @retval 0 Mapu nebylo mozne vytvorit.
 */
BombicMap * BombicMap::createCopy(int newWidth, int newHeight) {
	MapResourceHandler mapRH;
	return mapRH.createMapCopy(this,
		newWidth, newHeight, background_->name());
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
 * Dealokuje vsechny (pevne i generovane) objekty mapy a pozadi.
 */
BombicMap::~BombicMap() {
	// placed objects
	for(int x = fieldsRect_.left() ; x <= fieldsRect_.right() ; ++x) {
		for(int y = fieldsRect_.top() ; y <= fieldsRect_.bottom() ; ++y) {
			foreach(BombicMapObject * o, fields_[x][y].objList) {
				// delete it on the last (bottom-right) field
				if(o->rect().bottomRight() == Field(x, y)) {
					delete o;
				}
			}
			delete fields_[x][y].boxGen;
			delete fields_[x][y].creatureGen;
			delete fields_[x][y].bonusGen;
		}
	}
	// generated objects
	foreach(BombicMapObject * o, generatedBoxes_) {
		delete o;
	}
	foreach(BombicMapObject * o, generatedCreatures_) {
		delete o;
	}
	foreach(BombicMapObject * o, generatedBonuses_) {
		delete o;
	}
	// background
	delete background_;
}

/** @details
 * Kontroluje, zda se objekt vejde do mapy (podle BombicMapObject::size()
 * a BombicMapObject::toplapping()) a zda muze byt vkladany objekt na policku
 * zaroven s objekty, ktere jiz jsou na polickach, do kterych objekt zasahuje
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
 * Nekontroluje toplapping objektu, a ostatni objekty na zasazenych
 * polickach (viz BombicMap::canInsert()).
 * Pokud objekt blokuje generovani, obstara i blokovani generatoru.
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
			if(object->unblocksBonusGenerating()) {
				fields_[x][y].bonusGen->unblock();
			}
		}
	}
	modified_ = true;
}

/** @details
 * Odstrani objekt ze vsech policek, na kterych lezi.
 * Vlastnictvi objektu prechazi na volajiciho, objekt jiz
 * nebude dealokovan v destruktoru mapy.
 * Obnovuje blokovani generatoru.
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
	modified_ = true;
}

/** @details
 * Podle aktualne umistenych (i generovanych) objektu na policku @p field
 * nastavi blokovani generatoru.
 * @param field policko, pro ktere se ma obnovit blokovani
 */
void BombicMap::updateGeneratorsBlocking(const Field & field) {
	if(fieldsRect_.contains(field)) {
		updateGeneratorsBlocking(
			fields_[field.x()][field.y()]);
	}
}

/** @details
 * Podle aktualne umistenych (i generovanych) objektu na policku @p field
 * nastavi blokovani generatoru.
 * @param fieldSet policko (jeho obsah), pro ktere se ma obnovit blokovani
 */
void BombicMap::updateGeneratorsBlocking(const FieldSetT & fieldSet) {
	// check if some generator is blocking some other one
	bool blockBoxes =
		fieldSet.boxGen->blocksBoxGenerating() ||
		fieldSet.creatureGen->blocksBoxGenerating() ||
		fieldSet.bonusGen->blocksBoxGenerating();
	bool blockCreatures =
		fieldSet.boxGen->blocksCreatureGenerating() ||
		fieldSet.creatureGen->blocksCreatureGenerating() ||
		fieldSet.bonusGen->blocksCreatureGenerating();
	bool unblockBonuses =
		fieldSet.boxGen->unblocksBonusGenerating() ||
		fieldSet.creatureGen->unblocksBonusGenerating() ||
		fieldSet.bonusGen->unblocksBonusGenerating();
	// find the blocker
	foreach(BombicMapObject * o, fieldSet.objList) {
		if(o->blocksBoxGenerating()) {
			blockBoxes = true;
		}
		if(o->blocksCreatureGenerating()) {
			blockCreatures = true;
		}
		if(o->unblocksBonusGenerating()) {
			unblockBonuses = true;
		}
	}
	fieldSet.boxGen->setBlocking(blockBoxes);
	fieldSet.creatureGen->setBlocking(blockCreatures);
	fieldSet.bonusGen->setBlocking(!unblockBonuses);
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
const BombicMap::ObjectListT & BombicMap::objectsOnField(
		const BombicMap::Field & field) {
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
MapObjectGenerator * BombicMap::boxGenerator(
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
MapObjectGenerator * BombicMap::creatureGenerator(
		const BombicMap::Field & field) {
	if(!fieldsRect_.contains(field)) {
		return 0;
	}
	return fields_[field.x()][field.y()].creatureGen;
}

/** @details
 * Na kazdem policku vedeme zaznam o tom, zda lze na tomto
 * policku generovat bonusy. Toto je dotaz na strukturu konkretniho policka.
 * Vracena struktura zustava ve vlastnictvi mapy.
 * @param field policko, o nemz chceme informace
 * @return Generator bonusu.
 * @retval 0 Zadane policko neni v mape.
 */
MapObjectGenerator * BombicMap::bonusGenerator(
		const BombicMap::Field & field) {
	if(!fieldsRect_.contains(field)) {
		return 0;
	}
	return fields_[field.x()][field.y()].bonusGen;
}

/**
 * @return Seznam vsech beden, ktere maji byt v mape generovany.
 */
const BombicMap::ObjectListT & BombicMap::generatedBoxes() {
	return generatedBoxes_;
}
/**
 * @return Seznam vsech priser, ktere maji byt v mape generovany.
 */
const BombicMap::ObjectListT & BombicMap::generatedCreatures() {
	return generatedCreatures_;
}
/**
 * @return Seznam vsech bonusu, ktere maji byt v mape generovany.
 */
const BombicMap::ObjectListT & BombicMap::generatedBonuses() {
	return generatedBonuses_;
}

/** @details
 * Nastavi pocet beden stejneho druhu jako @p box pro generovani.
 * Tento pocet se tedy tyka beden,
 * ktere maji shodny @c BombicMapObject::name().
 * @param box prototyp bedny, jejiz pocet pro generovani nastavujeme
 * @param count novy pocet generovanych beden zadaneho druhu
 */
void BombicMap::setGeneratedBoxesCount(BombicMapObject * box, int count) {
	setGeneratedObjectsCount(generatedBoxes_, box, count);
}
/** @details
 * Nastavi pocet priser stejneho druhu jako @p creature pro generovani.
 * Tento pocet se tedy tyka priser,
 * ktere maji shodny @c BombicMapObject::name().
 * @param creature prototyp prisery, jejiz pocet pro generovani nastavujeme
 * @param count novy pocet generovanych priser zadaneho druhu
 */
void BombicMap::setGeneratedCreaturesCount(BombicMapObject * creature,
		int count) {
	setGeneratedObjectsCount(generatedCreatures_, creature, count);
}
/** @details
 * Nastavi pocet bonusu stejneho druhu jako @p bonus pro generovani.
 * Tento pocet se tedy tyka bonusu,
 * ktere maji shodny @c BombicMapObject::name().
 * @param bonus prototyp bonusu, jehoz pocet pro generovani nastavujeme
 * @param count novy pocet generovanych bonusu zadaneho druhu
 */
void BombicMap::setGeneratedBonusesCount(BombicMapObject * bonus, int count) {
	setGeneratedObjectsCount(generatedBonuses_, bonus, count);
}

/** @details
 * Nastavi pocet objektu stejneho druhu jako @p object pro generovani.
 * Tento pocet se tedy tyka objektu,
 * ktere maji shodny @c BombicMapObject::name().
 * @param objList seznam generovanych objektu
 * @param object prototyp objektu, jehoz pocet pro generovani nastavujeme
 * @param count novy pocet generovanych objektu zadaneho druhu
 */
void BombicMap::setGeneratedObjectsCount(ObjectListT & objList,
		BombicMapObject * object, int count) {
	// may take a while
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

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
	modified_ = true;

	QApplication::restoreOverrideCursor();
}

/** @details
 * Prida generovany objekt do seznamu.
 * Vlastnictvi objektu prechazi na mapu a objekt bude dealokovan
 * v destruktoru mapy nebo pri odstranovani ze seznamu.
 * Vyvola signal generatedMapObjectAdded().
 * @param objList seznam generovanych objektu
 * @param object pridavany objekt
 * @see removeGeneratedMapObject()
 */
void BombicMap::addGeneratedMapObject(ObjectListT & objList,
		BombicMapObject * object) {
	objList.append(object);
	switch(object->type()) {
		case BombicMapObject::Box:
			emit generatedBoxAdded(object);
			break;
		case BombicMapObject::Creature:
			emit generatedCreatureAdded(object);
			break;
		case BombicMapObject::Bonus:
			emit generatedBonusAdded(object);
			break;
		default:
			Q_ASSERT_X(false, "addGeneratedMapObject",
				"unhandled object type");
	}
}

/** @details
 * Odstrani generovany objekt ze seznamu.
 * Vyvola signal generatedMapObjectRemoved().
 * @param objList seznam generovanych objektu
 * @param object odstranovany objekt
 */
void BombicMap::removeGeneratedMapObject(ObjectListT & objList,
		BombicMapObject * object) {
	switch(object->type()) {
		case BombicMapObject::Box:
			emit generatedBoxRemoved(object);
			break;
		case BombicMapObject::Creature:
			emit generatedCreatureRemoved(object);
			break;
		case BombicMapObject::Bonus:
			emit generatedBonusRemoved(object);
			break;
		default:
			Q_ASSERT_X(false, "removeGeneratedMapObject",
				"unhandled object type");
	}
	objList.removeAll(object);
}

/**
 * @return Jmeno mapy.
 */
const QString & BombicMap::name() {
	return name_;
}

/**
 * @return Soubor, do ktereho mapu ulozit.
 */
const QString & BombicMap::filename() {
	return filename_;
}

/**
 * @param name nove jmeno mapy
 */
void BombicMap::setName(const QString & name) {
	name_ = name;
	modified_ = true;
}

/**
 * @param filename nove jmeno souboru
 */
void BombicMap::setFilename(const QString & filename) {
	filename_ = filename;
	modified_ = true;
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

/**
 * @return Zda byla mapa zmenena.
 */
bool BombicMap::wasModified() {
	return modified_;
}

/** @details
 * @param modified jestli byla mapa zmenena (novy stav mapy)
 */
void BombicMap::setModified(bool modified) {
	modified_ = modified;
}

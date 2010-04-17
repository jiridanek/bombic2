
#include "map_scene.h"

#include <QDebug>
#include <QBrush>
#include <QPen>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneDragDropEvent>
#include <QPoint>
#include <QRectF>
#include <QAction>
#include <QWizard>

#include <constants.h>

#include "map_object_palette.h"
#include "map_view.h"
#include "main_window.h"

#include "bombic/map.h"
#include "bombic/map_background.h"
#include "bombic/map_object.h"
#include "generators/map_object_generator.h"
#include "generators/generated_boxes_wizard.h"
#include "generators/generated_creatures_wizard.h"

/** @details
 * Vytvori scenu mapy @p map. Prebira vlastnictvi @p map a pri ruseni tuto mapu
 * dealokuje.
 * Nova scena se spoji (@c connect() ) s paletou objektu a s pohledem na scenu.
 * Pripravi se insertion helper items (zastupne graficke prvky pouzivane
 * pri vkladani objektu, kdyz nechceme zobrazit primo obrazek objektu).
 * @param map zobrazovana mapa
 * @param parent rodicovsky objekt
 */
MapScene::MapScene(BombicMap * map, QObject * parent):
				QGraphicsScene(parent), map_(map), workingObject_(0),
				insertionHelperItem_(new QGraphicsRectItem),
				selectedFieldHelperItem_(new QGraphicsRectItem),
				mousePressed_(false), mouseClicked_(false),
				doObjectGenerating_(true) {
	// init tools
	boxesGeneratingTools_.toGenerate =
		map->generatedBoxes();
	boxesGeneratingTools_.generatedObjectsWizard =
		new GeneratedBoxesWizard(map, MAIN_WINDOW);
	creaturesGeneratingTools_.toGenerate =
		map->generatedCreatures();
	creaturesGeneratingTools_.generatedObjectsWizard =
		new GeneratedCreaturesWizard(map, MAIN_WINDOW);

	// set the scene
	setSceneRect(QRect(QPoint(0, 0), map_->fieldsRect().size()*CELL_SIZE));
	setBackgroundBrush(map_->background()->ambientColor());
	// background of each field
	insertBackgroundFields();
	// map objects
	insertObjectsGraphicsItems();
	// generators
	initObjectGenerators();
	// do the generating
	generateObjects();

	// connect map to scene
	connect(map, SIGNAL(generatedBoxAdded(BombicMapObject *)),
		this, SLOT(addGeneratedBox(BombicMapObject *)) );
	connect(map, SIGNAL(generatedCreatureAdded(BombicMapObject *)),
		this, SLOT(addGeneratedCreature(BombicMapObject *)) );
	connect(map, SIGNAL(generatedBoxRemoved(BombicMapObject *)),
		this, SLOT(removeGeneratedBox(BombicMapObject *)) );
	connect(map, SIGNAL(generatedCreatureRemoved(BombicMapObject *)),
		this, SLOT(removeGeneratedCreature(BombicMapObject *)) );

	// connect palette to scene
	connect(MAP_OBJECT_PALETTE, SIGNAL(objectUnselected()),
		this, SLOT(unsetWorkingObject()) );
	connect(MAP_OBJECT_PALETTE, SIGNAL(objectSelected(BombicMapObject *)),
		this, SLOT(setWorkingObject(BombicMapObject *)) );

	// connect view to scene
	connect(MAP_VIEW, SIGNAL(leaved()),
		this, SLOT(hideWorkingObject()) );

	// connect menu actions to scene
	QAction * generateObjectsAction =
		MAIN_WINDOW->action(MainWindow::GenerateObjectsAction);
	generateObjectsAction->setChecked(doObjectGenerating_);
	connect(generateObjectsAction, SIGNAL(toggled(bool)),
		this, SLOT(toggleObjectGenerating()) );

	connect(MAIN_WINDOW->action(MainWindow::GeneratedBoxesAction),
		SIGNAL(triggered()),
		boxesGeneratingTools_.generatedObjectsWizard,
		SLOT(show()) );
	connect(MAIN_WINDOW->action(MainWindow::GeneratedCreaturesAction),
		SIGNAL(triggered()),
		creaturesGeneratingTools_.generatedObjectsWizard,
		SLOT(show()) );

	// init helper items as "insert item" and "item for selected field"
	initHelperItems();

	// if there is some selected object - unselect it at new scene
	MAP_OBJECT_PALETTE->unselectObject();
}

/** Loop over all fields of scene map.
 * Iterates with @p field over all fields of scene map.
 * Starting in top left goes over all columns and in every column
 * goes to bottom.
 * The loop variable @p field shouldn't be defined, because
 * it will be defined (and initialized) here.
 * @param field name of loop variable
 */
#define MAP_SCENE_FOREACH_MAP_FIELD(field) \
	for(BombicMap::Field field = map_->fieldsRect().topLeft() ; \
			field.x() <= map_->fieldsRect().right() ; \
			++field.rx()) \
		for(field.ry() = map_->fieldsRect().top() ; \
				field.y() <= map_->fieldsRect().bottom() ; \
				++field.ry())

/** @details
 * Prida a napozicuje graficke prvky pozadi mapy.
 */
void MapScene::insertBackgroundFields() {
	QPixmap texture = map_->background()->texture();
	MAP_SCENE_FOREACH_MAP_FIELD(f) {
		QGraphicsItem * bgItem = addPixmap(texture);
		bgItem->setPos(f*CELL_SIZE);
		bgItem->setZValue(-1);
	}
}

/** @details
 * Vlozi do sceny popisky generatoru a pevne umistenych objektu.
 */
void MapScene::insertObjectsGraphicsItems() {
	MAP_SCENE_FOREACH_MAP_FIELD(f) {
		// generators
		insertGeneratorGraphicsItem(
			map_->boxGenerator(f), 0.5 );
		insertGeneratorGraphicsItem(
			map_->creatureGenerator(f), 0.55 );
		// stable items
		foreach(BombicMapObject * o, map_->objectsOnField(f)) {
			if(o->graphicsItem()->scene()!=this) {
				// item is not in this scene
				addItem(o->situateGraphicsItem(
					f*CELL_SIZE ));
			}
		}
		sortGraphicsOnField(f);
	}
}

/**
 * @param generator generator, jehoz graficky prvek chceme pridat
 * @param zDiff hodnota ktera bude pridana k souradnici Z pri pozicovani
 */
void MapScene::insertGeneratorGraphicsItem(
		BombicMapObjectGenerator * generator, qreal zDiff) {

	QGraphicsItem * item = generator->graphicsItem();
	if(item->scene()!=this) {
		item->setZValue(sceneRect().height() + zDiff);
		addItem(item);
	}
}

/** @details
 * Inicializuje vsechny generatory mapy.
 * @see initObjectGenerator()
 */
void MapScene::initObjectGenerators() {
	MAP_SCENE_FOREACH_MAP_FIELD(f) {
		initObjectGenerator(
			map_->boxGenerator(f),
			boxesGeneratingTools_.availableGenerators,
			SLOT(registerBoxGeneratorChange()),
			SLOT(addGeneratedBox(BombicMapObject *)) );
		initObjectGenerator(
			map_->creatureGenerator(f),
			creaturesGeneratingTools_.availableGenerators,
			SLOT(registerCreatureGeneratorChange()),
			SLOT(addGeneratedCreature(BombicMapObject *)) );
	}
}

/** @details
 * Pripoji @p generator do slotu pro zmenu generatoru
 * a pro pridani generovaneho objektu.
 * Prida @p generator mezi generatory, ktere mohou generovat
 * samozrejme jen pokud muze generovat.
 * @param generator generator, ktery chceme inicializovat
 * @param availableGenerators seznam generatoru, ktere mohou generovat
 * @param registerGeneratorChangeMethod mistni slot pro zmenu generatoru
 * @param addGeneratedObjectMethod mistni slot pro pridani generovaneho objektu
 */
void MapScene::initObjectGenerator(
		BombicMapObjectGenerator * generator,
		ObjectGeneratorsT & availableGenerators,
		const char * registerGeneratorChangeMethod,
		const char * addGeneratedObjectMethod ) {

	connect(generator, SIGNAL(canGenerateChanged()),
		this, registerGeneratorChangeMethod);
	connect(generator, SIGNAL(removingGeneratedObject(BombicMapObject *)),
		this, addGeneratedObjectMethod );
	if(generator->canGenerate()) {
		availableGenerators.insert(generator);
	}
}

/**
 * @see registerGeneratorChange()
 */
void MapScene::registerBoxGeneratorChange() {
	registerGeneratorChange(
		qobject_cast<BombicMapObjectGenerator *>(sender()),
		boxesGeneratingTools_);
}

/**
 * @see registerGeneratorChange()
 */
void MapScene::registerCreatureGeneratorChange() {
	registerGeneratorChange(
		qobject_cast<BombicMapObjectGenerator *>(sender()),
		creaturesGeneratingTools_);
}

/** @details
 * Prida nebo odebere @p generator z @p availableGenerators.
 * Pokud jej pridal, zkusi vygenerovat objekty.
 * @param generator generator, ktery zmenu vyvolal
 * @param tools pomucky pro vygenerovani
 */
void MapScene::registerGeneratorChange(
		BombicMapObjectGenerator * generator,
		ObjectGeneratingToolsT & tools) {
	if(!generator) {
		return;
	}
	if(generator->canGenerate()) {
		tools.availableGenerators.insert(generator);
		generateObjects(tools);
	} else {
		tools.availableGenerators.remove(generator);
	}
}

/**
 * @param mapObj objekt, ktery ma byt pridan
 */
void MapScene::addGeneratedBox(BombicMapObject * mapObj) {
	addGeneratedObject(mapObj, boxesGeneratingTools_);
}
/**
 * @param mapObj objekt, ktery ma byt pridan
 */
void MapScene::addGeneratedCreature(BombicMapObject * mapObj) {
	addGeneratedObject(mapObj, creaturesGeneratingTools_);
}

/**
 * @param mapObj objekt, ktery ma byt pridan
 * @param tools pomucky pro vygenerovani
 */
void MapScene::addGeneratedObject( BombicMapObject * mapObj,
		ObjectGeneratingToolsT & tools) {
	// if the object was in map - update the old field
	// if no - some other field will be updated but it doesn't matter
	map_->updateGeneratorsBlocking(mapObj->field());

	tools.toGenerate.append(mapObj);
	generateObjects(tools);
}

/**
 * @param mapObj objekt, ktery ma byt odstranen
 */
void MapScene::removeGeneratedBox(BombicMapObject * mapObj) {
	removeGeneratedObject(mapObj, boxesGeneratingTools_);
}
/**
 * @param mapObj objekt, ktery ma byt odstranen
 */
void MapScene::removeGeneratedCreature(BombicMapObject * mapObj) {
	removeGeneratedObject(mapObj, creaturesGeneratingTools_);
}

/**
 * @param mapObj objekt, ktery ma byt odstranen
 * @param tools pomucky pro vygenerovani
 */
void MapScene::removeGeneratedObject(BombicMapObject * mapObj,
		ObjectGeneratingToolsT & tools) {
	if(!tools.toGenerate.contains(mapObj)) {
		// temporally disable generating
		bool oldDoObjectGenerating = doObjectGenerating_;
		doObjectGenerating_ = false;
		// the object must be generated somewhere in map
		map_->boxGenerator(mapObj->field())
			->removeGeneratedObjects();
		map_->creatureGenerator(mapObj->field())
			->removeGeneratedObjects();
		// turn back the object generating
		doObjectGenerating_ = oldDoObjectGenerating;
	}
	Q_ASSERT(tools.toGenerate.contains(mapObj));
	tools.toGenerate.removeAll(mapObj);
	// generate again the rest of removed
	generateObjects(tools);
}

/** @details
 * Prepne generovani objektu.
 * Objekty bud vygeneruje, nebo je naopak z mapy odstrani.
 */
void MapScene::toggleObjectGenerating() {
	doObjectGenerating_ = !doObjectGenerating_;
	if(doObjectGenerating_) {
		generateObjects();
	} else {
		removeGeneratedObjectsFromMap();
	}
}

/** @details
 * Vyhodi vsechny generovane objekty z mapy.
 * Pokud je zapnute generovani objektu,
 * tak ho na dobu odstranovani objektu vypne,
 * aby se objekty opet nevygenerovali.
 */
void MapScene::removeGeneratedObjectsFromMap() {
	bool oldDoObjectGenerating = doObjectGenerating_;
	doObjectGenerating_ = false;
	MAP_SCENE_FOREACH_MAP_FIELD(f) {
		map_->boxGenerator(f)
			->removeGeneratedObjects();
		map_->creatureGenerator(f)
			->removeGeneratedObjects();
	}
	doObjectGenerating_ = oldDoObjectGenerating;
}

/** @details
 * Pokusi se vygenerovat nevygenerovane objekty (bedny i prisery),
 * pokud nejake jsou.
 */
void MapScene::generateObjects() {
	generateObjects(boxesGeneratingTools_);
	generateObjects(creaturesGeneratingTools_);
}

/** @details
 * Pokusi se vygenerovat nevygenerovane objekty @p objectsToGenerate
 * do generatoru @p availableGenerators.
 * Pokud nejake (objekty i generatory) jsou.
 * Pokud je generovani zakazano (@c doObjectGenerating_ neni nastaveno)
 * nedela nic.
 * @param tools pomucky pro vygenerovani
 */
void MapScene::generateObjects(ObjectGeneratingToolsT & tools) {

	if(!doObjectGenerating_) {
		return;
	}

	while(!tools.toGenerate.isEmpty() &&
			!tools.availableGenerators.isEmpty()) {
		// get object and generator
		BombicMapObject * mapObj =
			takeRandomObject(tools.toGenerate);
		BombicMapObjectGenerator * generator =
			getRandomGenerator(tools.availableGenerators);
		// add the graphics
		QGraphicsItem * gi = mapObj->situateGraphicsItem(
			generator->field() * CELL_SIZE);
		if(gi->scene() != this) {
			addItem(gi);
		}
		// generate add object to generator (generate it)
		generator->addGeneratedObject(mapObj);
		map_->updateGeneratorsBlocking(mapObj->field());

		if(mapObj->sortOnField()) {
			sortGraphicsOnField(mapObj->field());
		}
	}
}

/** @details
 * Nahodne vybere jeden generator z @p generators.
 * Ten vrati ale zaroven ponecha v @p generators.
 * @param generators generatory, z nichz nahodne vybirame
 * @return Nahodne vybrany generator.
 */
BombicMapObjectGenerator * MapScene::getRandomGenerator(
		ObjectGeneratorsT & generators) {
	if(generators.isEmpty()) {
		return 0;
	}
	ObjectGeneratorsT::iterator it = generators.begin();
	for(int r = qrand() % generators.size() ; r > 0 ; --r) {
		// iterate to the r-th item
		++it;
	}
	return *it;
}

/** @details
 * Nahodne vybere jeden objekt mapy z @p objects a odstrani jej.
 * @param objects objekty, z nichz nahodne vybirame
 * @return Nahodne vybrany objekt mapy.
 */
BombicMapObject * MapScene::takeRandomObject(
		BombicMap::ObjectListT & objects) {
	if(objects.isEmpty()) {
		return 0;
	}
	int r = qrand() % objects.size();
	return objects.takeAt(r);
}

/** @details
 * Inicializuje prvek, ktery se pouzije,
 * kdyz uzivatel vklada do sceny objekt, ale nechceme zobrazit primo
 * tento objekt.
 * Napr. protoze na konkretni misto sceny to neni mozne.
 * Nechceme mast uzivatele duplikovanim obrazku objektu.
 * Dale inicializuje prvek, ktery se pouzije pro zvyrazneni policka,
 * ktere je detailne zobrazeno ve <em>field view</em>.
 * Po inicializaci prvky do sceny vlozi a skryje.
 */
void MapScene::initHelperItems() {
	insertionHelperItem_->setPen(HELPER_ITEM_PEN);
	insertionHelperItem_->setZValue(sceneRect().height()+2);
	insertionHelperItem_->hide();
	addItem(insertionHelperItem_);

	selectedFieldHelperItem_->setPen(HELPER_ITEM_PEN);
	selectedFieldHelperItem_->setBrush(SELECTED_FIELD_ITEM_BRUSH);
	selectedFieldHelperItem_->setZValue(sceneRect().height()+1);
	selectedFieldHelperItem_->setRect(0, 0, CELL_SIZE, CELL_SIZE);
	selectedFieldHelperItem_->hide();
	addItem(selectedFieldHelperItem_);
}

/** @details
 * Zobrazi na misto objektu, jehoz obdelnik je @p objectRect
 * pomocny graficky prvek, tak, jak byl naposledy nastaven.
 * Vetsinou chcete ale zaroven nastavit vyznam prvku, pouzijte proto radsi
 * @c showCanInsertItem() nebo @c showCannotInsertItem().
 * @param objectRect obdelnik objektu, ktery ma insertItem predstavovat
 */
void MapScene::showInsertionHelperItem(const QRect & objectRect) {
	insertionHelperItem_->setPos(objectRect.topLeft() * CELL_SIZE);
	insertionHelperItem_->setRect( QRectF(
		QPointF(0, 0),
		objectRect.size()*CELL_SIZE ) );
	insertionHelperItem_->show();
}

/**
 * @param objectRect obdelnik objektu, ktery ma insertItem predstavovat
 */
void MapScene::showCanInsertItem(const QRect & objectRect) {
	insertionHelperItem_->setBrush(CAN_INSERT_ITEM_BRUSH);
	showInsertionHelperItem(objectRect);
}

/**
 * @param objectRect obdelnik objektu, ktery ma insertItem predstavovat
 */
void MapScene::showCannotInsertItem(const QRect & objectRect) {
	insertionHelperItem_->setBrush(CANNOT_INSERT_ITEM_BRUSH);
	showInsertionHelperItem(objectRect);
}

/** @details
 * Dealokuje pruvodce nastaveni generovanych objektu.
 * Dealokuje mapu, ktera je ve scene zobrazena.
 */
MapScene::~MapScene() {
	delete boxesGeneratingTools_.generatedObjectsWizard;
	delete creaturesGeneratingTools_.generatedObjectsWizard;
	delete map_;
}

/** @details
 * Vlozenim objektu do sceny (mapy) se stava vlastnikem
 * objektu mapa, a objekt bude dealokovan pri dealokaci mapy.
 * @param object vkladany objekts
 * @param dstField policko, na ktere se ma objekt vlozit
 */
void MapScene::insert(BombicMapObject * object,
		const BombicMap::Field & dstField) {

	map_->insert(object, dstField);
	addItem(object->situateGraphicsItem( dstField*CELL_SIZE ));
	// update graphics items sorting
	if(object->sortOnField()) {
		sortGraphicsOnField(dstField);
	}
}

/** @details
 * Odstranenim objektu ze sceny (mapy) prechazi jeho vlastnictvi
 * na volajiciho. Objekt jiz nebude dealokovan pri dealokaci mapy.
 * @param object odstranovany objekt
 */
void MapScene::remove(BombicMapObject * object) {
	BombicMap::Field objField = object->field();

	removeItem(object->graphicsItem());
	map_->remove(object);

	if(object->sortOnField()) {
		sortGraphicsOnField(objField);
	}
}

/** @details
 * Kdybychom povolili vkladani vice stejnych objektu na jedno policko
 * a dale nic neresili, budou objekty na policku v zakrytu a nebude ani trochu
 * patrne, kolik asi jich tam je (a ktere). Tato fce napomuze tento problem
 * resit tim, ze vychyli objekty na policku tak, aby ta co je nahore
 * byla plne videt, a ostatni v zakrytu naznacovaly, kolik jich je.
 * @param field policko mapy, pro ktere chceme rozestaveni udelat
 */
void MapScene::sortGraphicsOnField(const BombicMap::Field & field) {
	// get generated creatures on field - they need to be sorted
	BombicMap::ObjectListT objects(
		map_->creatureGenerator(field)->generatedObjects() );

	// get placed creatures on field
	foreach(BombicMapObject * o, map_->objectsOnField(field)) {
		if(o->sortOnField()) {
			objects.append(o);
		}
	}

	if(objects.isEmpty()) {
		return;
	}
	// top whole part of half of count
	// count of creatures in one direction
	qreal halfCount = (objects.size()+1)/2;
	// maximal difference in one direction
	qreal maxDiff = qMin(halfCount*2.0, CELL_SIZE/4.0);
	// step from one to another creature
	qreal step = maxDiff / halfCount;
	// current difference
	qreal diff = maxDiff - step/2.0;
	// base position on field
	QPointF pos = field*CELL_SIZE;
	foreach(BombicMapObject * o, objects) {
		o->situateGraphicsItem(
			pos + QPointF(diff, -diff) );
		diff -= step;
	}
}

/** @details
 * Pri zmacknutem tlacitku mysi nastartuje tazeni objektu,
 * jinak hybne pracovnim objektem (je-li nejaky).
 * @param event udalost, ktera handler vyvolala
 */
void MapScene::mouseMoveEvent(QGraphicsSceneMouseEvent * event) {
	if(event->buttons().testFlag(Qt::LeftButton)) {
		hideWorkingObject();
		// the button is still pressed
		if(mousePressed_) {
			// I have here one press action to start dragging
			selectField(event->scenePos());
			startDragging(event);
		}
	} else {
		moveWorkingObject(event);
	}
	mousePressed_ = mouseClicked_ = false;
}

/** @details
 * TODO
 * @param event udalost, ktera handler vyvolala
 */
void MapScene::mousePressEvent(QGraphicsSceneMouseEvent * event) {
	switch(event->button()) {
		case Qt::LeftButton:
			mousePressed_ = true;
			break;
		case Qt::RightButton:
			// TODO show context menu
			break;
		default:
			// nothing to do
			break;
	}
}

/** @details
 * Pri predchozim stisku tlacitka (dohromady kliknuti)
 * vlozi do sceny (a mapy) pracovni objekt (je-li nejaky).
 * @param event udalost, ktera handler vyvolala
 */
void MapScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * event) {
	switch(event->button()) {
		case Qt::LeftButton:
			if(mousePressed_) {
				// it was a click
				mousePressed_ = false;
				mouseClicked_ = true;
				if(workingObject_) {
					insertWorkingObject(event);
				}
				selectField(event->scenePos());
			}
			break;
		default:
			// nothing to do
			break;
	}
}

/** @details
 * Odstrani ze sceny (a mapy) objekt, na ktery se dvojkliklo (je-li nejaky).
 * @param event udalost, ktera handler vyvolala
 */
void MapScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event) {
	switch(event->button()) {
		case Qt::LeftButton:
			mousePressed_ = false;
			if(mouseClicked_) {
				mouseClicked_ = false;
				removeClickedObject(event);
			}
			selectField(event->scenePos());
			break;
		default:
			// nothing to do
			break;
	}
}

/** @details
 * Zkontroluje je-li pod mysi nejaky objekt k tazeni
 * a nastartuje udalost tazeni tohoto objektu.
 * @param event udalost, ktera handler vyvolala
 */
void MapScene::startDragging(QGraphicsSceneMouseEvent * event) {
	BombicMap::Field eventField = getField(event->scenePos());
	BombicMapObject * draggedObj = map_->objectOnTop(eventField);
	if(draggedObj && draggedObj->canBeDragged()) {
		MapView::execDragging(event->widget(), draggedObj);
	}
}

/** @details
 * Je-li nejaky pracovni objekt, pohne s nim podle pozice mysi.
 * Lze-li objekt na tuto pozici vlozit, vlozi do sceny (ale nikoli do mapy)
 * primo tento objekt, v opacnem pripade zobrazi na teto pozici
 * zastupnou @c insertionHelperItem_ ve spravne velikosti a vyznamu cannot insert.
 * @param event udalost, ktera handler vyvolala
 */
void MapScene::moveWorkingObject(QGraphicsSceneMouseEvent * event) {
	if(!workingObject_) {
		return;
	}
	BombicMap::Field eventField = getField(
		event->scenePos(), workingObject_->rect());
	QGraphicsItem * workingGI = workingObject_->situateGraphicsItem(
		eventField*CELL_SIZE );
	if(map_->canInsert(workingObject_, eventField)) {
		insertionHelperItem_->hide();
		workingGI->show();
		if(workingGI->scene()!=this) {
			// working item is not in the right scene
			addItem(workingGI);
		}
	} else {
		workingGI->hide();

		showCannotInsertItem( QRect(
			eventField, workingObject_->size() ));
	}
}

/** @details
 * Je-li nejaky pracovni objekt, a lze-li jej na pozici mysi vlozit,
 * vlozi kopii pracovniho objektu do sceny (a mapy).
 * @param event udalost, ktera handler vyvolala
 */
void MapScene::insertWorkingObject(QGraphicsSceneMouseEvent * event) {
	if(!workingObject_) {
		return;
	}
	BombicMap::Field eventField = getField(
		event->scenePos(), workingObject_->rect());
	if(map_->canInsert(workingObject_, eventField)) {
		insert(workingObject_->createCopy(), eventField);
		// disable removing by next click
		mouseClicked_ = false;
	}
}

/** @details
 * Zkontroluje je-li pod mysi nejaky objekt k odstraneni
 * a pripadne jej odstrani (a dealokuje).
 * @param event udalost, ktera handler vyvolala
 */
void MapScene::removeClickedObject(QGraphicsSceneMouseEvent * event) {
	BombicMap::Field eventField = getField(event->scenePos());
	BombicMapObject * clickedObj = map_->objectOnTop(eventField);
	if(!clickedObj) {
		// nothing to remove
		return;
	}
	if(!clickedObj->canBeRemoved()) {
		return;
	}
	remove(clickedObj);
	delete clickedObj;
}

/** @details
 * Zkontroluje je-li pod mysi nejake policko a pripadne jej oznaci
 * jako vybrane a zobrazi jej v detailu v @c MapFieldView.
 * @param event udalost, ktera handler vyvolala
 */
void MapScene::selectField(const QPointF & eventPos) {
	selectedField_ = getField(eventPos);
	if(!map_->fieldsRect().contains(selectedField_)) {
		return;
	}

	selectedFieldHelperItem_->setPos(selectedField_*CELL_SIZE);
	selectedFieldHelperItem_->show();
	MAP_VIEW->updateFieldView();
}

void MapScene::unselectField() {
	selectedField_ = MAP_SCENE_FIELD_NOT_SELECTED;

	selectedFieldHelperItem_->hide();
	MAP_VIEW->updateFieldView();
}

/** @details
 * TODO vyhodit - nedela nic
 * @param event udalost, ktera handler vyvolala
 */
void MapScene::dragEnterEvent(QGraphicsSceneDragDropEvent * event) {
	Q_UNUSED(event);
}
/** @details
 * Mame-li nejaky tazeny objekt, zobrazi zastupny prvek ve spravne velikosti
 * podle toho jestli lze na tuto pozici mysi tazeny objekt premistit ci nikoli.
 * @param event udalost, ktera handler vyvolala
 */
void MapScene::dragMoveEvent(QGraphicsSceneDragDropEvent * event) {
	BombicMapObject * draggedObj =
		MapView::getMapObject(event->mimeData());
	if(!draggedObj) {
		// nothing was dragged
		return;
	}
	BombicMap::Field eventField = getField(
		event->scenePos(), draggedObj->rect() );
	QRect objectRect(eventField, draggedObj->size());
	if(map_->canInsert(draggedObj, eventField)) {
		showCanInsertItem(objectRect);
	} else {
		showCannotInsertItem(objectRect);
	}
}
/** @details
 * Schova prvky znazornujici tazeny objekt.
 * @param event udalost, ktera handler vyvolala
 */
void MapScene::dragLeaveEvent(QGraphicsSceneDragDropEvent * event) {
	Q_UNUSED(event);
	insertionHelperItem_->hide();
}

/** @details
 * Mame-li nejaky tazeny objekt a lze jej na pozici mysi vlozit,
 * presune tazeny objekt. Schova prvny znazornujici tazeny objekt.
 * @param event udalost, ktera handler vyvolala
 */
void MapScene::dropEvent(QGraphicsSceneDragDropEvent * event) {
	BombicMapObject * draggedObj =
		MapView::getMapObject(event->mimeData());
	if(!draggedObj) {
		// nothing was dragged
		return;
	}
	BombicMap::Field eventField = getField(
		event->scenePos(), draggedObj->rect() );
	if(map_->canInsert(draggedObj, eventField)) {
		remove(draggedObj);
		insert(draggedObj, eventField);
		selectField(event->scenePos());
	}
	insertionHelperItem_->hide();
}

/** @details
 * Mame objekt urcite velikosti (defaultne 1x1 policko).
 * Objekt si predstavime vycentrovany pod pozici mysi.
 * Zajima nas umisteni leveho horniho policka objektu.
 * @param mousePosition pozice mysi v pixelech
 * @param relatedObjectRect obdelik objektu, ktereho se akce mysi tyka
 * @return Leve horni policko objektu, kdyz si ho predstavime pod mysi.
 */
BombicMap::Field MapScene::getField(
		const QPointF & mousePosition,
		const QRect & relatedObjectRect) {
	QPoint eventPoint = mousePosition.toPoint();
	QPoint relativeMiddle(
		relatedObjectRect.width(),
		relatedObjectRect.height() );
	relativeMiddle *= CELL_SIZE/2;
	return (eventPoint - relativeMiddle) / CELL_SIZE;
}

/** @details
 * Vlastnictvi pracovniho objektu zustava volajicimu.
 * Nez volajici zrusi ci modifikuje tento objekt,
 * mel by vzdy zavola unsetWorkingObject().
 * @param object nastavovany objekt
 */
void MapScene::setWorkingObject(BombicMapObject * object) {
	if(workingObject_ == object) {
		return;
	}
	workingObject_ = object;
	MAP_VIEW->showWorkingObjectLabel(object);
}

/** @details
 * Vlastnikem pracovniho objektu neni MapScene ale volajici,
 * tato fce tedy jen zrusi referenci na pracovni objekt.
 */
void MapScene::unsetWorkingObject() {
	if(!workingObject_) {
		return;
	}
	hideWorkingObject();
	MAP_VIEW->hideWorkingObjectLabel();
	workingObject_ = 0;
}

/** @details
 * Odstrani ze sceny pracovni objekt (je-li nejaky) a skryje
 * jeho pripadny zastupny prvek @c insertionHelperItem_.
 */
void MapScene::hideWorkingObject() {
	if(!workingObject_) {
		return;
	}
	QGraphicsItem * workingGI = workingObject_->graphicsItem();
	if(workingGI->scene()==this) {
		// working item is in the right scene
		removeItem(workingGI);
	}

	insertionHelperItem_->hide();
}



#include "map_scene.h"

#include <QBrush>
#include <QPen>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneDragDropEvent>
#include <QDrag>
#include <QPoint>
#include <QRectF>

#include <constants.h>

#include "map_object_palette.h"
#include "map_view.h"

#include "bombic/map.h"
#include "bombic/map_background.h"
#include "bombic/map_object.h"
#include "bombic/wall.h"
#include "bombic/generated_object.h"

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
				mousePressed_(false) {
	// set the scene
	setSceneRect(QRect(QPoint(0, 0), map_->fieldsRect().size()*CELL_SIZE));
	setBackgroundBrush(map_->background()->ambientColor());
	// background of each field
	insertBackgroundFields();
	// map objects
	insertObjectsGraphicsItems();

	// connect palette to scene
	connect(MAP_OBJECT_PALETTE, SIGNAL(objectUnselected()),
		this, SLOT(unsetWorkingObject()) );
	connect(MAP_OBJECT_PALETTE, SIGNAL(objectSelected(BombicMapObject *)),
		this, SLOT(setWorkingObject(BombicMapObject *)) );

	// connect view to scene
	connect(MAP_VIEW, SIGNAL(leaved()),
		this, SLOT(hideWorkingObject()) );

	// init the "insert item"
	initInsertionHelperItem();
}

/** @details
 * @param texture textura pozadi mapy
 */
void MapScene::insertBackgroundFields() {
	QRect mapRect = map_->fieldsRect();
	QPixmap texture = map_->background()->texture();
	for(BombicMap::Field f = mapRect.topLeft() ;
			f.x() <= mapRect.right() ; ++f.rx()) {
		for(f.ry() = mapRect.top() ;
				f.y() <= mapRect.bottom() ; ++f.ry()) {
			QGraphicsItem * bgItem = addPixmap(texture);
			bgItem->setPos(f*CELL_SIZE);
			bgItem->setZValue(-1);
		}
	}
}

/** @details
 */
void MapScene::insertObjectsGraphicsItems() {
	QRect mapRect = map_->fieldsRect();
	for(BombicMap::Field f = mapRect.topLeft() ;
			f.x() <= mapRect.right() ; ++f.rx()) {
		for(f.ry() = mapRect.top() ;
				f.y() <= mapRect.bottom() ; ++f.ry()) {
			// generated items
			QGraphicsItem * item =
				map_->generatedBox(f)->graphicsItem();
			item->setZValue(sceneRect().height()+0.5);
			addItem(item);
			item = map_->generatedCreature(f)->graphicsItem();
			item->setZValue(sceneRect().height()+0.55);
			addItem(item);
			// stable items
			foreach(BombicMapObject * o, map_->objectsOnField(f)) {
				if(o->graphicsItem()->scene()!=this) {
					// item is not in this scene
					addItem(o->situateGraphicsItem(
						f*CELL_SIZE ));
				}
			}
		}
	}
}

/** @details
 * Inicializuje a vlozi prveky sceny, ktery se pouziji,
 * kdyz uzivatel vklada do sceny objekt, ale nechceme zobrazit primo objekt.
 * Napr. protoze na konkretni misto sceny to neni mozne.
 * Nechceme mast uzivatele duplikovanim obrazku objektu.
 * Po inicializaci skryte prvky do sceny vlozi.
 */
void MapScene::initInsertionHelperItem() {
	insertionHelperItem_->setPen(INSERTION_HELPER_ITEM_PEN);
	insertionHelperItem_->setZValue(sceneRect().height()+1);
	insertionHelperItem_->hide();
	addItem(insertionHelperItem_);
}

/** @details
 * Zobrazi na misto objektu, jehoz obdelnik je @p objectRect
 * pomocny graficky prvek, tak, jak byl naposledy nastaven.
 * Vetsinou chcete ale zaroven nastavit vyznam prvku, pouzijte proto radsi
 * @c showCanInsertItem() nebo @c showCannotInsertItem().
 */
void MapScene::showInsertionHelperItem(const QRect & objectRect) {
	insertionHelperItem_->setPos(objectRect.topLeft() * CELL_SIZE);
	insertionHelperItem_->setRect( QRectF(
		QPointF(0, 0),
		objectRect.size()*CELL_SIZE ) );
	insertionHelperItem_->show();
}

void MapScene::showCanInsertItem(const QRect & objectRect) {
	insertionHelperItem_->setBrush(CAN_INSERT_ITEM_BRUSH);
	showInsertionHelperItem(objectRect);
}

void MapScene::showCannotInsertItem(const QRect & objectRect) {
	insertionHelperItem_->setBrush(CANNOT_INSERT_ITEM_BRUSH);
	showInsertionHelperItem(objectRect);
}

/** @details
 * Dealokuje i mapu, ktera je ve scene zobrazena.
 */
MapScene::~MapScene() {
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
}

/** @details
 * Odstranenim objektu ze sceny (mapy) prechazi jeho vlastnictvi
 * na volajiciho. Objekt jiz nebude dealokovan pri dealokaci mapy.
 * @param object odstranovany objekt
 */
void MapScene::remove(BombicMapObject * object) {

	removeItem(object->graphicsItem());
	map_->remove(object);
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
			mousePressed_ = false;
			startDragging(event);
		}
	} else {
		moveWorkingObject(event);
	}
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
				insertWorkingObject(event);
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
			removeClickedObject(event);
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
	if(!draggedObj) {
		// nothing to be dragged
		return;
	}
	if(!draggedObj->canBeDragged()) {
		return;
	}

	QDrag * drag = new QDrag(event->widget());
	drag->setMimeData(MapView::createMimeData(draggedObj));
	drag->setPixmap(draggedObj->thumbnail());
	drag->start();
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
		eventField*CELL_SIZE);
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


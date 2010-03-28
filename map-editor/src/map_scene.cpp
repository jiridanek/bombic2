
#include "map_scene.h"

#include <QBrush>
#include <QPen>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneDragDropEvent>
#include <QDrag>
#include <QPoint>
#include <QPointF>
#include <QRect>

#include <constants.h>

#include "map_object_palette.h"
#include "map_view.h"

#include "bombic/map.h"
#include "bombic/map_background.h"
#include "bombic/map_object.h"
#include "bombic/wall.h"

/** @details
 * Vytvori scenu mapy s velikosti @p width, @p height v polickach
 * a pozadim @p background. Vytvori (a drzi) mapu tak, jak je pouzita
 * ve hre. Nova scena se spoji ( @c connect() ) s paletou objektu
 * a s pohledem na scenu. Pripravi se @c cantInsertItem_.
 * @param width sirska nove mapy v polickach
 * @param height vyska nove mapy v polickach
 * @param background pozadi nove mapy
 * @param parent rodicovsky objekt
 */
MapScene::MapScene(int width, int height,
		BombicMapBackground * background,
		QObject * parent):
				QGraphicsScene(parent), workingObject_(0),
				cantInsertItem_(new QGraphicsRectItem),
				mousePressed_(false) {
	// create clean map
	map_ = new BombicMap(width, height, background);
	// set the scene
	setSceneRect(0, 0, width*CELL_SIZE, height*CELL_SIZE);
	setBackgroundBrush(background->ambientColor());
	insertBackgroundFields(background->texture());
	// insert enclosure walls
	insertBackgroundWalls(background);

	// connect palette to scene
	connect(MAP_OBJECT_PALETTE, SIGNAL(objectUnselected()),
		this, SLOT(unsetWorkingObject()) );
	connect(MAP_OBJECT_PALETTE, SIGNAL(objectSelected(BombicMapObject *)),
		this, SLOT(setWorkingObject(BombicMapObject *)) );

	// connect view to scene
	connect(MAP_VIEW, SIGNAL(leaved()),
		this, SLOT(hideWorkingObject()) );

	// init the "cant insert item"
	initCantInsertItem();
}

/** @details
 * @param texture textura pozadi mapy
 */
void MapScene::insertBackgroundFields(const QPixmap & texture) {
	for(int x = 0 ; x < sceneRect().width() ; x += CELL_SIZE) {
		for(int y = 0 ; y < sceneRect().height() ; y += CELL_SIZE) {
			QGraphicsItem * bgItem = addPixmap(texture);
			bgItem->setPos(x, y);
			bgItem->setZValue(-1);
		}
	}
}

/** @details
 * Do sceny a mapy vlozi obvodove zdi definovane pozadim @p background.
 * @param background pozadi vytvarene mapy
 */
void MapScene::insertBackgroundWalls(BombicMapBackground * background) {
	QRect mapRect = map_->fieldsRect();

	#define PREPARE_WALL_AND_RECT(situation, moveMethod, getMethod) \
		BombicWall * wall = background->getWall( \
		BombicMapBackground::situation); \
		/* create wall rect with right size (anywhere) */ \
		QRect wallRect(QPoint(0,0), wall->size()); \
		/* move the wall rect to the corner of map */ \
		wallRect.moveMethod(mapRect.getMethod());

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
					mapRect.contains(field) ; \
					field += BombicMap::Field(stepX, stepY) ) { \
				/* insert wall if it is possible */ \
				if(map_->canInsert(wall, field)) { \
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
 * Inicializuje a vlozi prvek sceny, ktery se pouzije,
 * kdyz uzivatel vklada do sceny objekt, ale na konkretni misto
 * sceny to neni mozne. Po inicializaci skryty prvek do sceny vlozi.
 */
void MapScene::initCantInsertItem() {
	cantInsertItem_->setPen(CANT_INSERT_ITEM_PEN);
	cantInsertItem_->setBrush(CANT_INSERT_ITEM_BRUSH);
	cantInsertItem_->setZValue(sceneRect().height()+1);
	cantInsertItem_->hide();
	addItem(cantInsertItem_);
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
	if(mousePressed_) {
		mousePressed_ = false;
		startDragging(event);
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
	hideWorkingObject();

	QDrag * drag = new QDrag(event->widget());
	drag->setMimeData(MapView::createMimeData(draggedObj));
	drag->start();
}

/** @details
 * Je-li nejaky pracovni objekt, pohne s nim podle pozice mysi.
 * Lze-li objekt na tuto pozici vlozit, vlozi do sceny (ale nikoli do mapy)
 * primo tento objekt, v opacnem pripade zobrazi na teto pozici
 * zastupnou cantInsertItem_ ve spravne velikosti.
 * @param event udalost, ktera handler vyvolala
 */
void MapScene::moveWorkingObject(QGraphicsSceneMouseEvent * event) {
	if(!workingObject_) {
		return;
	}
	BombicMap::Field eventField = getField(
		event->scenePos(), workingObject_->rect());
	QPointF insertionPoint = eventField*CELL_SIZE;
	QGraphicsItem * workingGI = workingObject_->situateGraphicsItem(
		insertionPoint);
	if(map_->canInsert(workingObject_, eventField)) {
		cantInsertItem_->hide();
		workingGI->show();
		if(workingGI->scene()!=this) {
			// working item is not in the right scene
			addItem(workingGI);
		}
	} else {
		workingGI->hide();

		cantInsertItem_->setPos(insertionPoint);
		cantInsertItem_->setRect( QRectF(
			QPointF(0, 0),
			workingObject_->size()*CELL_SIZE ) );
		cantInsertItem_->show();
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
}
/** @details
 * Schova prvky znazornujici tazeny objekt.
 * @param event udalost, ktera handler vyvolala
 */
void MapScene::dragLeaveEvent(QGraphicsSceneDragDropEvent * event) {
}

/** @details
 * Mame-li nejaky tazeny objekt a lze jej na pozici mysi vlozit,
 * presune tazeny objekt.
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
	MAP_VIEW->showWorkingObjectLabel(object->pixmap());
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
 * jeho pripadny zastupny prvek @c cantInsertItem_.
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

	cantInsertItem_->hide();
}


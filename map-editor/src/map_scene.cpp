
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

MapScene::MapScene(int width, int height,
		BombicMapBackground * background,
		QObject * parent):
				QGraphicsScene(parent), workingObject_(0),
				cantInsertItem_(new QGraphicsRectItem),
				mousePressed_(false) {
	// set the scene
	setSceneRect(0, 0, width*CELL_SIZE, height*CELL_SIZE);
	setBackgroundBrush(background->ambientColor());
	insertBackgroundFields(background->texture());
	// create clean map
	map_ = new BombicMap(width, height, background);
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

	// set the "cant insert item"
	cantInsertItem_->setPen(CANT_INSERT_ITEM_PEN);
	cantInsertItem_->setBrush(CANT_INSERT_ITEM_BRUSH);
	cantInsertItem_->setZValue(sceneRect().height()+1);
	cantInsertItem_->hide();
	addItem(cantInsertItem_);
}

void MapScene::insertBackgroundFields(const QPixmap & texture) {
	for(int x = 0 ; x < sceneRect().width() ; x += CELL_SIZE) {
		for(int y = 0 ; y < sceneRect().height() ; y += CELL_SIZE) {
			QGraphicsItem * bgItem = addPixmap(texture);
			bgItem->setPos(x, y);
			bgItem->setZValue(-1);
		}
	}
}

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
}

MapScene::~MapScene() {
	delete map_;
}

void MapScene::insert(BombicMapObject * object,
		const BombicMap::Field & dstField) {

	map_->insert(object, dstField);
	addItem(object->situateGraphicsItem( dstField*CELL_SIZE ));
}

void MapScene::remove(BombicMapObject * object) {

	removeItem(object->graphicsItem());
	map_->remove(object);
}

void MapScene::mouseMoveEvent(QGraphicsSceneMouseEvent * event) {
	if(mousePressed_) {
		mousePressed_ = false;
		startDragging(event);
	} else {
		moveWorkingObject(event);
	}
}

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

void MapScene::dragEnterEvent(QGraphicsSceneDragDropEvent * event) {
	Q_UNUSED(event);
}
void MapScene::dragMoveEvent(QGraphicsSceneDragDropEvent * event) {
}
void MapScene::dragLeaveEvent(QGraphicsSceneDragDropEvent * event) {
}
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

void MapScene::setWorkingObject(BombicMapObject * object) {
	if(workingObject_ == object) {
		return;
	}
	workingObject_ = object;
	MAP_VIEW->showWorkingObjectLabel(object->pixmap());
}

void MapScene::unsetWorkingObject() {
	if(!workingObject_) {
		return;
	}
	hideWorkingObject();
	MAP_VIEW->hideWorkingObjectLabel();
	workingObject_ = 0;
}

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



#include "map_scene.h"

#include <QBrush>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
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
				QGraphicsScene(parent), workingObject_(0) {
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

void MapScene::mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent) {
	if(!workingObject_) {
		return;
	}
	BombicMap::Field eventField = getEventField(mouseEvent);
	if(map_->canInsert(workingObject_, eventField)) {
		QGraphicsItem * workingGI = workingObject_->situateGraphicsItem(
			eventField*CELL_SIZE);
		if(workingGI->scene()!=this) {
			// working item is not in the right scene
			addItem(workingGI);
		}
	} else {
		hideWorkingObject();
	}
}

void MapScene::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent) {
	if(!workingObject_) {
		return;
	}
	BombicMap::Field eventField = getEventField(mouseEvent);
	if(map_->canInsert(workingObject_, eventField)) {
		insert(workingObject_->createCopy(), eventField);
	}
}

BombicMap::Field MapScene::getEventField(
		QGraphicsSceneMouseEvent * mouseEvent) {
	QPoint eventPoint = mouseEvent->scenePos().toPoint();
	QPoint relativeMiddle(CELL_SIZE/2, CELL_SIZE/2);
	if(workingObject_) {
		relativeMiddle.setX(workingObject_->size().width());
		relativeMiddle.setY(workingObject_->size().height());
		relativeMiddle *= CELL_SIZE/2;
	}
	return (eventPoint - relativeMiddle) / CELL_SIZE;
}

void MapScene::setWorkingObject(BombicMapObject * object) {
	if(workingObject_ == object) {
		return;
	}
	workingObject_ = object;
	MAP_VIEW->showWorkingObjectLabel(object->pixmap());
	// TODO
}

void MapScene::unsetWorkingObject() {
	if(!workingObject_) {
		return;
	}
	hideWorkingObject();
	MAP_VIEW->hideWorkingObjectLabel();
	workingObject_ = 0;
	// TODO
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
}

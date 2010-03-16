
#include "map_scene.h"

#include <QBrush>
#include <QGraphicsItem>
#include <QPoint>
#include <QRect>

#include <constants.h>

#include "bombic/map.h"
#include "bombic/map_background.h"
#include "bombic/map_object.h"
#include "bombic/wall.h"

MapScene::MapScene(int width, int height,
		BombicMapBackground * background,
		QObject * parent):
				QGraphicsScene(parent) {
	// set the scene
	setSceneRect(0, 0, width*CELL_SIZE, height*CELL_SIZE);
	setBackgroundBrush(
		QColor(background->getWall(BombicMapBackground::TopLeft)
			->pixmap().toImage().pixel(0,0) ) );
	QPixmap bgTexture = background->texture();
	for(int x = 0 ; x < width*CELL_SIZE ; x += CELL_SIZE) {
		for(int y = 0 ; y < height*CELL_SIZE ; y += CELL_SIZE) {
			QGraphicsItem * bgItem = addPixmap(bgTexture);
			bgItem->setPos(x, y);
			bgItem->setZValue(-1);
		}
	}
	// create clean map
	map_ = new BombicMap(width, height, background);
	// insert enclosure walls
	QRect mapRect = map_->fieldsRect();
	BombicWall * wall = 0;
	// corners
	#define CORNER(situation, moveMethod, getMethod) \
		do { \
			wall = background->getWall(BombicMapBackground::situation); \
			/* create wall rect with right size (anywhere) */ \
			QRect wallRect(QPoint(0,0), wall->size()); \
			/* move the wall rect to the corner of map */ \
			wallRect.moveMethod(mapRect.getMethod()); \
			/* insert wall (by the top left corner of wall rect) */ \
			insert(wall, wallRect.topLeft() ); \
		} while(0)
	CORNER(TopLeft, moveTopLeft, topLeft);
	CORNER(TopRight, moveTopRight, topRight);
	CORNER(BottomLeft, moveBottomLeft, bottomLeft);
	CORNER(BottomRight, moveBottomRight, bottomRight);
	#undef CORNER
	// whole sides
	#define SIDE(situation, moveMethod, getMethod, stepX, stepY) \
		do { \
			wall = background->getWall(BombicMapBackground::situation); \
			/* create wall rect with right size (anywhere) */ \
			QRect wallRect(QPoint(0,0), wall->size()); \
			/* move the wall rect to the corner of map */ \
			wallRect.moveMethod(mapRect.getMethod()); \
			/* along whole side */ \
			for(BombicMap::Field field = wallRect.topLeft() ; \
					mapRect.contains(field) ; \
					field += BombicMap::Field(stepX, stepY) ) { \
				/* insert wall if it is possible */ \
				if(map_->canInsert(wall, field)) { \
					insert(wall, field); \
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
	addItem(object->createGraphicsItem( dstField*CELL_SIZE ));
}

void MapScene::mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent) {
}

void MapScene::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent) {
}


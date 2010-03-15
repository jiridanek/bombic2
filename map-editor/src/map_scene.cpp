
#include "map_scene.h"

#include <QBrush>
#include <QGraphicsItem>

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
	setBackgroundBrush( QBrush(background->texture()) );
	// create clean map
	map_ = new BombicMap(width, height, background);
	// insert enclosure walls
	QRectF adjScene =
		sceneRect().adjusted(0, 0, -CELL_SIZE+1, -CELL_SIZE+1);
	BombicWall * wall = 0;
	wall = background->getWall(BombicMapBackground::TopLeft);
	addItem(wall->createGraphicsItem( adjScene.topLeft() ));
	wall = background->getWall(BombicMapBackground::TopRight);
	addItem(wall->createGraphicsItem( adjScene.topRight() ));
	wall = background->getWall(BombicMapBackground::BottomLeft);
	addItem(wall->createGraphicsItem( adjScene.bottomLeft() ));
	wall = background->getWall(BombicMapBackground::BottomRight);
	addItem(wall->createGraphicsItem( adjScene.bottomRight() ));


}


MapScene::~MapScene() {
	delete map_;
}

void MapScene::mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent) {
}

void MapScene::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent) {
}


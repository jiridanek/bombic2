
#include "map_scene.h"

#include <QBrush>

#include <constants.h>

#include "bombic/map.h"
#include "bombic/map_background.h"
#include "bombic/map_object.h"

MapScene::MapScene(QObject * parent):
		QGraphicsScene(parent) {

	// TODO init
}

MapScene::MapScene(int width, int height,
		BombicMapBackground * background,
		QObject * parent):
				QGraphicsScene(parent) {

	init(width, height, background);
}

void MapScene::init(int width, int height,
		BombicMapBackground * background) {

	setSceneRect(0, 0, width*CELL_SIZE, height*CELL_SIZE);
	setBackgroundBrush( QBrush(background->texture()) );
	map_ = new BombicMap(width, height, background);
}


MapScene::~MapScene() {
	delete map_;
}

void MapScene::mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent) {
}

void MapScene::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent) {
}


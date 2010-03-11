
#include "map_view.h"

#include <QGraphicsView>

#include "bombic/map.h"
#include "bombic/map_object.h"

SINGLETON_INIT(MapView);

MapView::MapView(QObject * parent):
		QGraphicsScene(parent), viewport_(0), map_(0) {

	SINGLETON_CONSTRUCT;
	viewport_ = new QGraphicsView(this);
}
MapView::~MapView() {
	SINGLETON_DESTROY;
}

QGraphicsView * MapView::getViewport() {
	return viewport_;
}

void MapView::mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent) {
}

void MapView::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent) {
}



#include "mapview.h"

#define MAPVIEW_DEFAULT_GEOMETRY QRectF(0, 0, 800, 600)
#define MAPVIEW_NEEDED_VIEWPORT_BORDER QSize(30, 30)

MapView::MapView(QObject * parent):
		QGraphicsScene(MAPVIEW_DEFAULT_GEOMETRY, parent) {

	viewport_ = new QGraphicsView(this);
	viewport_->setMaximumSize(
		sceneRect().toRect().size() + MAPVIEW_NEEDED_VIEWPORT_BORDER);
}

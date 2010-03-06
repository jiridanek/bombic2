
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsPixmapItem>

#include "mapview.h"

#define MAPVIEW_DEFAULT_GEOMETRY QRectF(0, 0, 800, 600)
#define MAPVIEW_NEEDED_VIEWPORT_BORDER QSize(30, 30)

MapView::MapView(QObject * parent, MapObjectPalette * palette):
		QGraphicsScene(MAPVIEW_DEFAULT_GEOMETRY, parent),
		palette_(palette), itemToBeAdded_(0) {

	viewport_ = new QGraphicsView(this);
	viewport_->setMaximumSize(
		sceneRect().toRect().size() + MAPVIEW_NEEDED_VIEWPORT_BORDER);
}

void MapView::mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent) {
	if(palette_->hasSelectedObject()) {
		if(!itemToBeAdded_) {
			itemToBeAdded_ = addPixmap(palette_->getSelectedObject());
		} else {
			itemToBeAdded_->setPixmap(palette_->getSelectedObject());
		}
		itemToBeAdded_->setOffset(0, -50); // it is the toplapping
		QPoint eventPoint = mouseEvent->scenePos().toPoint();
		// set position to rounded point
		itemToBeAdded_->setPos( ( (eventPoint - QPoint(25, 25) )/50)*50 );
		itemToBeAdded_->setZValue(itemToBeAdded_->y());
	}
}

void MapView::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent) {
	if(mouseEvent->button() != Qt::LeftButton) {
		return;
	}

	if(palette_->hasSelectedObject()) {
		// copy the item
		QGraphicsPixmapItem * newItem = new QGraphicsPixmapItem;
		newItem->setPixmap(itemToBeAdded_->pixmap());
		newItem->setOffset(itemToBeAdded_->offset());
		newItem->setPos(itemToBeAdded_->pos());
		newItem->setZValue(itemToBeAdded_->zValue());
		addItem(newItem);
	}
}
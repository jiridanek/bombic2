
#ifndef MAPVIEW_H_GUARD
#define MAPVIEW_H_GUARD


#include <QGraphicsScene>
#include <QGraphicsView>

#include "mapobjectpalette.h"

class MapView: public QGraphicsScene {

public:
	MapView(QObject * parent, MapObjectPalette * palette);

	QGraphicsView * getViewport() {
		return viewport_;
	}

protected:
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent);
	virtual void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);

private:
	QGraphicsView * viewport_;
	MapObjectPalette * palette_;
	QGraphicsPixmapItem * itemToBeAdded_;
};


#endif

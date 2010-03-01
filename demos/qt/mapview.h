
#ifndef MAPVIEW_H_GUARD
#define MAPVIEW_H_GUARD


#include <QGraphicsScene>
#include <QGraphicsView>

class MapView: public QGraphicsScene {

public:
	MapView(QObject * parent);

	QGraphicsView * getViewport() {
		return viewport_;
	}
private:
	QGraphicsView * viewport_;

};


#endif


#include "map_view.h"

#include <QGraphicsView>
#include <QGridLayout>

#include "map_scene.h"

#include "bombic/map.h"
#include "bombic/map_object.h"

SINGLETON_INIT(MapView);

MapView::MapView(QWidget * parent):
		QWidget(parent), viewport_(0), scene_(0) {

	SINGLETON_CONSTRUCT;
}

MapView::MapView(int width, int height,
		BombicMapBackground * background,
		QWidget * parent): QWidget(parent) {

	SINGLETON_CONSTRUCT;

	scene_ = new MapScene(width, height, background, this);
	viewport_ = new QGraphicsView(scene_);

	gridLayout()->addWidget(viewport_, 0, 1);
}
MapView::~MapView() {
	SINGLETON_DESTROY;
}

QGridLayout * MapView::gridLayout() {
	QLayout * myLayout = layout();
	if(myLayout) {
		return static_cast<QGridLayout *>( myLayout );
	} else {
		return new QGridLayout(this);
	}

}
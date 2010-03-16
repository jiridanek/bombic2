
#include "map_view.h"

#include <QGraphicsView>
#include <QGridLayout>
#include <QSlider>

#include <constants.h>

#include "map_scene.h"

#include "resource_handler.h"
#include "bombic/map.h"
#include "bombic/map_background.h"
#include "bombic/map_object.h"

SINGLETON_INIT(MapView);

MapView::MapView(QWidget * parent):
		QWidget(parent), viewport_(0), scene_(0) {

	SINGLETON_CONSTRUCT;

	BombicMapBackground * defaultMapBg =
		RESOURCE_HANDLER->loadMapBackground(DEFAULT_MAP_BACKGROUND);
	if(defaultMapBg) {
		scene_ = new MapScene(DEFAULT_MAP_WIDTH, DEFAULT_MAP_HEIGHT,
			defaultMapBg, this);
		viewport_ = new QGraphicsView(scene_);
		gridLayout()->addWidget(viewport_, 1, 0, 1, 2);

		zoomSlider_ = new QSlider(Qt::Horizontal, this);
		gridLayout()->addWidget(zoomSlider_, 2, 1);
		connect(zoomSlider_, SIGNAL(valueChanged(int)),
			this, SLOT(setZoom(int)) );
		zoomSlider_->setRange(30, 150);
		zoomSlider_->setValue(100);
	}
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

void MapView::setZoom(int zoom) {
	qreal z = zoom/100.0;
	viewport_->resetTransform();
	viewport_->scale(z, z);
}
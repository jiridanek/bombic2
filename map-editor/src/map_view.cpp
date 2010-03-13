
#include "map_view.h"

#include <QGraphicsView>
#include <QGridLayout>

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

	scene_ = new MapScene(DEFAULT_MAP_WIDTH, DEFAULT_MAP_HEIGHT,
		defaultMapBg, this);
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


#include "map_view.h"

#include <QGraphicsView>
#include <QGridLayout>
#include <QLabel>
#include <QEvent>

#include <constants.h>

#include "map_scene.h"

#include "qt/zoomwidget.h"
#include "resource_handler.h"
#include "bombic/map.h"
#include "bombic/map_background.h"
#include "bombic/map_object.h"

SINGLETON_INIT(MapView);

MapView::MapView(QWidget * parent):
		QWidget(parent), viewport_(0), scene_(0),
		zoomWidget_(0), lastZoomQuotient_(1.0) {

	SINGLETON_CONSTRUCT;

	viewport_ = new QGraphicsView;
	gridLayout()->addWidget(viewport_, 1, 0, 1, 2);

	workingObjectLabel_ = new QLabel;
	workingObjectLabel_->setScaledContents(true);
	workingObjectLabel_->setMaximumHeight(CELL_SIZE);
	gridLayout()->addWidget(workingObjectLabel_, 2, 0);

	zoomWidget_ = new ZoomWidget(ZOOM_STEP,
		ZOOM_MINIMUM_VALUE, ZOOM_MAXIMUM_VALUE);
	connect(zoomWidget_, SIGNAL(zoomChanged(qreal)),
		this, SLOT(setZoom(qreal)) );
	gridLayout()->addWidget(zoomWidget_, 2, 1);


	BombicMapBackground * defaultMapBg =
		RESOURCE_HANDLER->loadMapBackground(DEFAULT_MAP_BACKGROUND);
	if(defaultMapBg) {
		scene_ = new MapScene(DEFAULT_MAP_WIDTH, DEFAULT_MAP_HEIGHT,
			defaultMapBg, this);
		viewport_->setScene(scene_);
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

void MapView::setZoom(qreal zoomQuotient) {
	if(zoomQuotient==1.0) {
		viewport_->resetTransform();
	} else {
		qreal dz = zoomQuotient/lastZoomQuotient_;
		viewport_->scale(dz, dz);
	}
	lastZoomQuotient_ = zoomQuotient;
}

void MapView::leaveEvent(QEvent * event) {
	event->accept();
	emit leaved();
}

void MapView::showWorkingObjectLabel(const QPixmap & objectPixmap) {
	workingObjectLabel_->setPixmap(objectPixmap);
	QSize pixmapSize = objectPixmap.size();
	int labelWidth = pixmapSize.width()
		* workingObjectLabel_->maximumHeight() / pixmapSize.height();
	workingObjectLabel_->setMaximumWidth(labelWidth);
	workingObjectLabel_->show();
}
void MapView::hideWorkingObjectLabel() {
	workingObjectLabel_->hide();
}
